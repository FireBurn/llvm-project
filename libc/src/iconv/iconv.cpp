//===-- Implementation of iconv -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/iconv/iconv.h"

#include "hdr/errno_macros.h"
#include "hdr/stdint_proxy.h"
#include "hdr/types/char32_t.h"
#include "hdr/types/iconv_t.h"
#include "hdr/types/size_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/iconv/conversion.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

using iconv_internal::Conversion;
using iconv_internal::Status;

size_t fail(int error) {
  libc_errno = error;
  return static_cast<size_t>(-1);
}

// POSIX says a character with no place in the target set is EILSEQ, the same
// as input which is not a character at all.
size_t fail_with(Status status) {
  return fail(status == Status::FULL ? E2BIG : EILSEQ);
}

// Writes one character as the target set has it. A byte order mark written
// ahead of the character stays written, whatever becomes of the character.
// Under //IGNORE, a character with no place in the target set is left out.
Status write_plain(Conversion &conv, char32_t cp, char **outbuf,
                   size_t *outbytesleft, bool &skipped) {
  size_t made = 0;
  auto out = reinterpret_cast<unsigned char *>(*outbuf);
  Status status = iconv_internal::encode(conv, cp, out, *outbytesleft, made);
  *outbuf += made;
  *outbytesleft -= made;
  if (status == Status::INVALID && conv.ignore) {
    skipped = true;
    return Status::OK;
  }
  return status;
}

// Writes the code for a sequence of characters, high byte first.
Status write_code(uint16_t code, char **outbuf, size_t *outbytesleft) {
  const size_t length = code > 0xFF ? 2 : 1;
  if (*outbytesleft < length)
    return Status::FULL;
  auto out = reinterpret_cast<unsigned char *>(*outbuf);
  if (length == 2)
    *out++ = static_cast<unsigned char>(code >> 8);
  *out = static_cast<unsigned char>(code & 0xFF);
  *outbuf += length;
  *outbytesleft -= length;
  return Status::OK;
}

// Writes the characters held back in case they began a sequence: as the
// sequence they are, if they are one, or else one at a time.
Status write_held(Conversion &conv, char **outbuf, size_t *outbytesleft,
                  bool &skipped) {
  const iconv_internal::Sequence *whole = nullptr;
  iconv_internal::match(*conv.to_sequences, conv.write_held,
                        conv.write_held_count, whole);
  if (whole != nullptr) {
    Status status = write_code(whole->code, outbuf, outbytesleft);
    if (status == Status::OK)
      conv.write_held_count = 0;
    return status;
  }
  while (conv.write_held_count > 0) {
    Status status =
        write_plain(conv, conv.write_held[0], outbuf, outbytesleft, skipped);
    if (status != Status::OK)
      return status;
    conv.write_held[0] = conv.write_held[1];
    --conv.write_held_count;
  }
  return Status::OK;
}

// Writes one character. A full output is reported before whether the
// character has a place in the target set. Where the target set has a code for
// a sequence of characters, a character which may begin one is held back
// until the characters after it show whether it does.
Status write_character(Conversion &conv, char32_t cp, char **outbuf,
                       size_t *outbytesleft, bool &skipped) {
  if (*outbytesleft < iconv_internal::narrowest(conv.to))
    return Status::FULL;
  if (conv.to_sequences == nullptr)
    return write_plain(conv, cp, outbuf, outbytesleft, skipped);

  char32_t wanted[3];
  size_t count = 0;
  for (; count < conv.write_held_count; ++count)
    wanted[count] = conv.write_held[count];
  wanted[count++] = cp;
  const iconv_internal::Sequence *whole = nullptr;
  switch (iconv_internal::match(*conv.to_sequences, wanted, count, whole)) {
  case iconv_internal::Match::PREFIX:
    conv.write_held[conv.write_held_count++] = cp;
    return Status::OK;
  case iconv_internal::Match::WHOLE: {
    Status status = write_code(whole->code, outbuf, outbytesleft);
    if (status == Status::OK)
      conv.write_held_count = 0;
    return status;
  }
  default:
    break;
  }
  if (conv.write_held_count == 0)
    return write_plain(conv, cp, outbuf, outbytesleft, skipped);
  // What was held begins no sequence with this character, which may begin one
  // of its own.
  Status status = write_held(conv, outbuf, outbytesleft, skipped);
  if (status != Status::OK)
    return status;
  return write_character(conv, cp, outbuf, outbytesleft, skipped);
}

// Writes the characters still owed from a code which stands for several.
Status write_pending(Conversion &conv, char **outbuf, size_t *outbytesleft,
                     bool &skipped) {
  while (conv.pending_count > 0) {
    Status status =
        write_character(conv, conv.pending[0], outbuf, outbytesleft, skipped);
    if (status != Status::OK)
      return status;
    conv.pending[0] = conv.pending[1];
    --conv.pending_count;
  }
  return Status::OK;
}

} // namespace

LLVM_LIBC_FUNCTION(size_t, iconv,
                   (iconv_t cd, char **__restrict inbuf,
                    size_t *__restrict inbytesleft, char **__restrict outbuf,
                    size_t *__restrict outbytesleft)) {
  if (cd == reinterpret_cast<iconv_t>(-1) || cd == nullptr)
    return fail(EBADF);
  auto &conv = *reinterpret_cast<Conversion *>(cd);

  // Whether anything was left out under //IGNORE.
  bool skipped = false;

  // Going back to the initial state. With somewhere to write it, the output
  // first gets what it owes: the rest of a code which stands for several
  // characters, a character held back for a mark which did not come,
  // characters held back for a sequence which did not come, and the end of an
  // open UTF-7 run. Without, that is dropped. A byte order mark may then be
  // read again and is owed again, but the byte order already read is kept, as
  // glibc does.
  if (inbuf == nullptr || *inbuf == nullptr) {
    if (outbuf != nullptr && *outbuf != nullptr) {
      Status status = write_pending(conv, outbuf, outbytesleft, skipped);
      if (status == Status::OK && conv.held != 0) {
        status =
            write_character(conv, conv.held, outbuf, outbytesleft, skipped);
        if (status == Status::OK)
          conv.held = 0;
      }
      if (status == Status::OK && conv.write_held_count > 0)
        status = write_held(conv, outbuf, outbytesleft, skipped);
      if (status == Status::OK) {
        size_t made = 0;
        auto out = reinterpret_cast<unsigned char *>(*outbuf);
        status = iconv_internal::unshift(conv, out, *outbytesleft, made);
        *outbuf += made;
        *outbytesleft -= made;
      }
      if (status != Status::OK)
        return fail_with(status);
    }
    const bool big = conv.read_big;
    iconv_internal::reset_state(conv);
    conv.read_big = big;
    return skipped ? fail(EILSEQ) : 0;
  }

  // Where the input goes back to when a character cannot be written: just
  // after the last one which was. As in glibc, input which was only held back
  // since then is handed back too, and read again by the next call.
  char *kept_in = *inbuf;
  size_t kept_left = *inbytesleft;
  char32_t kept_held = conv.held;
  auto keep = [&]() {
    kept_in = *inbuf;
    kept_left = *inbytesleft;
    kept_held = conv.held;
  };
  // A write which got as far as writing something, such as characters held
  // back for a sequence, keeps the input that came from.
  auto give_back = [&](Status status, const char *before) {
    if (*outbuf != before)
      keep();
    *inbuf = kept_in;
    *inbytesleft = kept_left;
    conv.held = kept_held;
    return fail_with(status);
  };

  while (*inbytesleft > 0) {
    Status status = write_pending(conv, outbuf, outbytesleft, skipped);
    if (status != Status::OK)
      return fail_with(status);

    char32_t cp = 0;
    size_t used = 0;
    auto in = reinterpret_cast<const unsigned char *>(*inbuf);
    switch (iconv_internal::decode(conv, in, *inbytesleft, cp, used)) {
    case Status::NONE:
      *inbuf += used;
      *inbytesleft -= used;
      keep();
      continue;
    case Status::INCOMPLETE:
      return fail(EINVAL);
    case Status::INVALID:
      if (!conv.ignore)
        return fail(EILSEQ);
      *inbuf += used;
      *inbytesleft -= used;
      skipped = true;
      keep();
      continue;
    case Status::SEQUENCE: {
      // The code is used once the first of its characters is written, and the
      // rest are owed.
      const auto *sequence = iconv_internal::find_code(
          *conv.from_sequences, static_cast<uint16_t>(cp));
      if (sequence == nullptr)
        return fail(EILSEQ);
      const char *before = *outbuf;
      status = write_character(conv, sequence->code_points[0], outbuf,
                               outbytesleft, skipped);
      if (status != Status::OK)
        return give_back(status, before);
      *inbuf += used;
      *inbytesleft -= used;
      keep();
      for (size_t i = 1; i < sequence->length; ++i)
        conv.pending[conv.pending_count++] = sequence->code_points[i];
      status = write_pending(conv, outbuf, outbytesleft, skipped);
      if (status != Status::OK)
        return fail_with(status);
      continue;
    }
    default:
      break;
    }

    if (conv.from_combining != nullptr) {
      const auto &combining = *conv.from_combining;
      char32_t joined = 0;
      if (conv.held != 0)
        joined = iconv_internal::compose(combining, conv.held, cp);
      bool wrote = false;
      if (joined != 0) {
        cp = joined;
        conv.held = 0;
      } else if (conv.held != 0) {
        // No mark joins the held character, so it goes out ahead of this one.
        const char *before = *outbuf;
        status =
            write_character(conv, conv.held, outbuf, outbytesleft, skipped);
        if (status != Status::OK)
          return give_back(status, before);
        conv.held = 0;
        keep();
        wrote = true;
      }
      if (iconv_internal::takes_marks(combining, cp)) {
        conv.held = cp;
        *inbuf += used;
        *inbytesleft -= used;
        if (wrote)
          keep();
        continue;
      }
    }

    const char *before = *outbuf;
    status = write_character(conv, cp, outbuf, outbytesleft, skipped);
    if (status != Status::OK)
      return give_back(status, before);

    // The input is only consumed once the output has been written, so a
    // caller which is handed E2BIG can enlarge the buffer and go again.
    *inbuf += used;
    *inbytesleft -= used;
    keep();
  }

  // What was left out is still an error once the rest has been converted, so
  // that a caller can tell the output is not the whole of the input.
  if (skipped)
    return fail(EILSEQ);

  // Every character converted to exactly one character, so none of them was
  // converted in a way which cannot be undone.
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

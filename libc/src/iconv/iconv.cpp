//===-- Implementation of iconv -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/iconv/iconv.h"

#include "hdr/errno_macros.h"
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

// Writes one character. A full output is reported before whether the character
// has a place in the target set, and a byte order mark written ahead of the
// character stays written, whatever becomes of the character.
Status write_character(Conversion &conv, char32_t cp, char **outbuf,
                       size_t *outbytesleft) {
  if (*outbytesleft < iconv_internal::narrowest(conv.to))
    return Status::FULL;
  size_t made = 0;
  auto out = reinterpret_cast<unsigned char *>(*outbuf);
  Status status = iconv_internal::encode(conv, cp, out, *outbytesleft, made);
  *outbuf += made;
  *outbytesleft -= made;
  return status;
}

} // namespace

LLVM_LIBC_FUNCTION(size_t, iconv,
                   (iconv_t cd, char **__restrict inbuf,
                    size_t *__restrict inbytesleft, char **__restrict outbuf,
                    size_t *__restrict outbytesleft)) {
  if (cd == reinterpret_cast<iconv_t>(-1) || cd == nullptr)
    return fail(EBADF);
  auto &conv = *reinterpret_cast<Conversion *>(cd);

  // Going back to the initial state. With somewhere to write it, the output
  // first gets what it owes: a character held back for a mark which did not
  // come, then the end of an open UTF-7 run. Without, that is dropped. A byte
  // order mark may then be read again and is owed again, but the byte order
  // already read is kept, as glibc does.
  if (inbuf == nullptr || *inbuf == nullptr) {
    bool skipped = false;
    if (outbuf != nullptr && *outbuf != nullptr) {
      if (conv.held != 0) {
        Status status = write_character(conv, conv.held, outbuf, outbytesleft);
        if (status == Status::FULL)
          return fail(E2BIG);
        if (status != Status::OK && !conv.ignore)
          return fail(EILSEQ);
        skipped = status != Status::OK;
        conv.held = 0;
      }
      size_t made = 0;
      auto out = reinterpret_cast<unsigned char *>(*outbuf);
      if (iconv_internal::unshift(conv, out, *outbytesleft, made) ==
          Status::FULL)
        return fail(E2BIG);
      *outbuf += made;
      *outbytesleft -= made;
    }
    const bool big = conv.read_big;
    iconv_internal::reset_state(conv);
    conv.read_big = big;
    return skipped ? fail(EILSEQ) : 0;
  }

  // Whether anything was left out under //IGNORE.
  bool skipped = false;

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
  auto give_back = [&](int error) {
    *inbuf = kept_in;
    *inbytesleft = kept_left;
    conv.held = kept_held;
    return fail(error);
  };

  while (*inbytesleft > 0) {
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
        Status status = write_character(conv, conv.held, outbuf, outbytesleft);
        if (status == Status::FULL)
          return give_back(E2BIG);
        if (status != Status::OK) {
          if (!conv.ignore)
            return give_back(EILSEQ);
          skipped = true;
        }
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

    Status status = write_character(conv, cp, outbuf, outbytesleft);
    if (status == Status::FULL)
      return give_back(E2BIG);
    if (status != Status::OK) {
      // The character has no place in the target set. POSIX says this is
      // EILSEQ, the same as an input which was not a character at all.
      if (!conv.ignore)
        return give_back(EILSEQ);
      skipped = true;
    }

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

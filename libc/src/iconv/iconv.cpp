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

LLVM_LIBC_FUNCTION(size_t, iconv,
                   (iconv_t cd, char **__restrict inbuf,
                    size_t *__restrict inbytesleft, char **__restrict outbuf,
                    size_t *__restrict outbytesleft)) {
  if (cd == reinterpret_cast<iconv_t>(-1) || cd == nullptr) {
    libc_errno = EBADF;
    return static_cast<size_t>(-1);
  }
  auto &conv = *reinterpret_cast<iconv_internal::Conversion *>(cd);

  // Going back to the initial state lets a byte order mark be read again and
  // makes the output owe one again. The byte order already read is kept, as
  // glibc does, and none of these sets need any bytes written to get back.
  if (inbuf == nullptr || *inbuf == nullptr) {
    conv.read_mark = true;
    conv.write_mark = true;
    return 0;
  }

  // Whether anything was left out under //IGNORE.
  bool skipped = false;

  while (*inbytesleft > 0) {
    char32_t cp = 0;
    size_t used = 0;
    auto in = reinterpret_cast<const unsigned char *>(*inbuf);
    switch (iconv_internal::decode(conv, in, *inbytesleft, cp, used)) {
    case iconv_internal::Status::NONE:
      *inbuf += used;
      *inbytesleft -= used;
      continue;
    case iconv_internal::Status::INCOMPLETE:
      libc_errno = EINVAL;
      return static_cast<size_t>(-1);
    case iconv_internal::Status::INVALID:
      if (conv.ignore) {
        *inbuf += used;
        *inbytesleft -= used;
        skipped = true;
        continue;
      }
      libc_errno = EILSEQ;
      return static_cast<size_t>(-1);
    default:
      break;
    }

    // Whether the input was a character is reported first, and a full output
    // before whether the character has a place in the target set.
    if (*outbytesleft < iconv_internal::narrowest(conv.to)) {
      libc_errno = E2BIG;
      return static_cast<size_t>(-1);
    }

    size_t made = 0;
    auto out = reinterpret_cast<unsigned char *>(*outbuf);
    auto wrote = iconv_internal::encode(conv, cp, out, *outbytesleft, made);
    // A byte order mark written ahead of the character stays written, whatever
    // becomes of the character.
    *outbuf += made;
    *outbytesleft -= made;
    if (wrote == iconv_internal::Status::FULL) {
      libc_errno = E2BIG;
      return static_cast<size_t>(-1);
    }
    if (wrote != iconv_internal::Status::OK) {
      // The character has no place in the target set. POSIX says this is
      // EILSEQ, the same as an input which was not a character at all.
      if (conv.ignore) {
        *inbuf += used;
        *inbytesleft -= used;
        skipped = true;
        continue;
      }
      libc_errno = EILSEQ;
      return static_cast<size_t>(-1);
    }

    // The input is only consumed once the output has been written, so a
    // caller which is handed E2BIG can enlarge the buffer and go again.
    *inbuf += used;
    *inbytesleft -= used;
  }

  // What was left out is still an error once the rest has been converted, so
  // that a caller can tell the output is not the whole of the input.
  if (skipped) {
    libc_errno = EILSEQ;
    return static_cast<size_t>(-1);
  }

  // Every character converted to exactly one character, so none of them was
  // converted in a way which cannot be undone.
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

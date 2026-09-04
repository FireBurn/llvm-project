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
  const auto &conv = *reinterpret_cast<iconv_internal::Conversion *>(cd);

  // None of these conversions carry state between calls, so being asked to
  // go back to the initial state is nothing to do.
  if (inbuf == nullptr || *inbuf == nullptr)
    return 0;

  while (*inbytesleft > 0) {
    char32_t cp = 0;
    size_t used = 0;
    auto in = reinterpret_cast<const unsigned char *>(*inbuf);
    switch (iconv_internal::decode(conv, in, *inbytesleft, cp, used)) {
    case iconv_internal::Status::INCOMPLETE:
      libc_errno = EINVAL;
      return static_cast<size_t>(-1);
    case iconv_internal::Status::INVALID:
      libc_errno = EILSEQ;
      return static_cast<size_t>(-1);
    default:
      break;
    }

    size_t made = 0;
    auto out = reinterpret_cast<unsigned char *>(*outbuf);
    auto wrote = iconv_internal::encode(conv, cp, out, *outbytesleft, made);
    if (wrote == iconv_internal::Status::FULL) {
      libc_errno = E2BIG;
      return static_cast<size_t>(-1);
    }
    if (wrote != iconv_internal::Status::OK) {
      // The character has no place in the target set. POSIX says this is
      // EILSEQ, the same as an input which was not a character at all.
      libc_errno = EILSEQ;
      return static_cast<size_t>(-1);
    }

    // The input is only consumed once the output has been written, so a
    // caller which is handed E2BIG can enlarge the buffer and go again.
    *inbuf += used;
    *inbytesleft -= used;
    *outbuf += made;
    *outbytesleft -= made;
  }

  // Every character converted to exactly one character, so none of them was
  // converted in a way which cannot be undone.
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

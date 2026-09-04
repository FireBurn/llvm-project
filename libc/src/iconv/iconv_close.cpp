//===-- Implementation of iconv_close -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/iconv/iconv_close.h"

#include "hdr/errno_macros.h"
#include "hdr/func/free.h"
#include "hdr/types/iconv_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/iconv/conversion.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, iconv_close, (iconv_t cd)) {
  if (cd == reinterpret_cast<iconv_t>(-1) || cd == nullptr) {
    libc_errno = EBADF;
    return -1;
  }
  ::free(cd);
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

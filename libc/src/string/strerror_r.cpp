//===-- Implementation of strerror_r --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/string/strerror_r.h"
#include "src/__support/StringUtil/error_to_string.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/properties/os.h"

#include <stddef.h>

#ifdef LIBC_TARGET_OS_IS_ANDROID
#include "src/__support/StringUtil/xsi_strerror_r.h"
#endif

namespace LIBC_NAMESPACE_DECL {

#ifdef LIBC_TARGET_OS_IS_ANDROID
// bionic's strerror_r is the POSIX one, and Android code (Rust's std among it)
// calls it as such. The GNU one is __gnu_strerror_r.
LLVM_LIBC_FUNCTION(int, strerror_r, (int err_num, char *buf, size_t buflen)) {
  return xsi_strerror_r(err_num, buf, buflen);
}
#else
// This is the gnu version of strerror_r. The XSI version is __xpg_strerror_r.
LLVM_LIBC_FUNCTION(char *, strerror_r,
                   (int err_num, char *buf, size_t buflen)) {
  return const_cast<char *>(get_error_cstring(err_num, {buf, buflen}));
}
#endif

} // namespace LIBC_NAMESPACE_DECL

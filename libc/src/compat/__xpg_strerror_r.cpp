//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __xpg_strerror_r
///
//===----------------------------------------------------------------------===//

#include "src/compat/__xpg_strerror_r.h"

#include "src/__support/StringUtil/xsi_strerror_r.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// There are two strerror_r: the one POSIX states, which fills the buffer and
// answers with an error number, and the one GNU states, which answers with a
// string that may or may not be the buffer. strerror_r is the GNU one here,
// as it is there, and this name is what the other one goes by. On Android
// strerror_r is the POSIX one, as it is in bionic, and __gnu_strerror_r the
// GNU one.
LLVM_LIBC_FUNCTION(int, __xpg_strerror_r,
                   (int err_num, char *buf, size_t buflen)) {
  return xsi_strerror_r(err_num, buf, buflen);
}

} // namespace LIBC_NAMESPACE_DECL

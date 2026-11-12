//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __gnu_strerror_r
///
//===----------------------------------------------------------------------===//

#include "src/compat/__gnu_strerror_r.h"

#include "src/__support/StringUtil/error_to_string.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include <stddef.h>

namespace LIBC_NAMESPACE_DECL {

// The strerror_r GNU states. On Android this is what strerror_r is called when
// _GNU_SOURCE is defined, as in bionic.
LLVM_LIBC_FUNCTION(char *, __gnu_strerror_r,
                   (int err_num, char *buf, size_t buflen)) {
  return const_cast<char *>(get_error_cstring(err_num, {buf, buflen}));
}

} // namespace LIBC_NAMESPACE_DECL

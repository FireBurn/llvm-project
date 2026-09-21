//===-- Implementation of __ctype_get_mb_cur_max --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/compat/__ctype_get_mb_cur_max.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/mb_cur_max.h"

namespace LIBC_NAMESPACE_DECL {

// Both glibc and musl define MB_CUR_MAX as a call to this, so code built
// against either reaches for the name rather than the variable this libc's
// own MB_CUR_MAX names. It is the same number.
LLVM_LIBC_FUNCTION(size_t, __ctype_get_mb_cur_max, (void)) {
  return __llvm_libc_mb_cur_max;
}

} // namespace LIBC_NAMESPACE_DECL

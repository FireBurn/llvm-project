//===-- How many bytes a character takes ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/mb_cur_max.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// One until a locale says otherwise, which is what the C locale means.
LLVM_LIBC_VARIABLE(size_t, __llvm_libc_mb_cur_max) = 1;

namespace internal {

void set_mb_cur_max(size_t bytes) { __llvm_libc_mb_cur_max = bytes; }

} // namespace internal

} // namespace LIBC_NAMESPACE_DECL

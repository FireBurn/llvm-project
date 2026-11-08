//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of malloc_backtrace.
///
//===----------------------------------------------------------------------===//

#include "src/android/malloc_backtrace.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// bionic returns 0 as well when malloc debug is not loaded: Scudo records no
// allocation backtraces to hand out.
LLVM_LIBC_FUNCTION(ssize_t, malloc_backtrace, (void *, uintptr_t *, size_t)) {
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

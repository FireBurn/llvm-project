//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for malloc_backtrace.
///
//===----------------------------------------------------------------------===//

#include "src/android/malloc_backtrace.h"
#include "test/UnitTest/Test.h"

#include <stdint.h>

TEST(LlvmLibcMallocBacktraceTest, NoBacktraceIsKept) {
  int object = 0;
  uintptr_t frames[8] = {};
  EXPECT_EQ(LIBC_NAMESPACE::malloc_backtrace(&object, frames, 8), ssize_t(0));
  EXPECT_EQ(frames[0], uintptr_t(0));
}

TEST(LlvmLibcMallocBacktraceTest, NullArgumentsAreAccepted) {
  EXPECT_EQ(LIBC_NAMESPACE::malloc_backtrace(nullptr, nullptr, 0), ssize_t(0));
}

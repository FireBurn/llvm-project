//===-- Unittests for malloc_trim -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/func/mallopt.h"
#include "include/llvm-libc-macros/malloc-macros.h"
#include "src/stdlib/malloc_trim.h"
#include "test/UnitTest/Test.h"

namespace {

int last_option = 0;
int last_value = 0;
int answer = 0;

} // anonymous namespace

// The allocator is whatever defines mallopt, which on Linux is scudo and is
// not linked into a test. What malloc_trim does is pass the request on, so
// that is what is checked, against a mallopt of the test's own.
extern "C" int mallopt(int option, int value) noexcept {
  last_option = option;
  last_value = value;
  return answer;
}

TEST(LlvmLibcMallocTrimTest, AsksTheAllocatorToPurge) {
  last_option = 0;
  last_value = -1;
  answer = 1;

  ASSERT_EQ(LIBC_NAMESPACE::malloc_trim(0), 1);
  ASSERT_EQ(last_option, M_PURGE);
  ASSERT_EQ(last_value, 0);
}

// One means something was handed back, zero that there was nothing to hand
// back. Both are answers rather than errors, and both are passed on.
TEST(LlvmLibcMallocTrimTest, ReportsWhatTheAllocatorSaid) {
  answer = 0;
  ASSERT_EQ(LIBC_NAMESPACE::malloc_trim(0), 0);
}

// The padding to leave behind is advice this allocator has no way to be
// told, so it is dropped rather than passed on as a size.
TEST(LlvmLibcMallocTrimTest, IgnoresThePadding) {
  answer = 1;
  last_value = -1;

  ASSERT_EQ(LIBC_NAMESPACE::malloc_trim(1 << 16), 1);
  ASSERT_EQ(last_option, M_PURGE);
  ASSERT_EQ(last_value, 0);
}

//===-- Unittests for reallocarray ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "src/__support/libc_errno.h"
#include "src/stdlib/reallocarray.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcReallocarrayTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcReallocarrayTest, GrowsAndKeepsTheContents) {
  int *ptr = reinterpret_cast<int *>(::malloc(4 * sizeof(int)));
  ASSERT_TRUE(ptr != nullptr);
  for (int i = 0; i < 4; ++i)
    ptr[i] = i + 1;

  int *bigger = reinterpret_cast<int *>(
      LIBC_NAMESPACE::reallocarray(ptr, 16, sizeof(int)));
  ASSERT_TRUE(bigger != nullptr);
  ASSERT_ERRNO_SUCCESS();
  for (int i = 0; i < 4; ++i)
    EXPECT_EQ(bigger[i], i + 1);
  ::free(bigger);
}

TEST_F(LlvmLibcReallocarrayTest, NullPointerAllocates) {
  void *ptr = LIBC_NAMESPACE::reallocarray(nullptr, 8, sizeof(long));
  ASSERT_TRUE(ptr != nullptr);
  ::free(ptr);
}

TEST_F(LlvmLibcReallocarrayTest, OverflowFailsWithoutFreeingTheBlock) {
  int *ptr = reinterpret_cast<int *>(::malloc(4 * sizeof(int)));
  ASSERT_TRUE(ptr != nullptr);
  ptr[0] = 42;

  // A product which does not fit in a size_t must be reported rather than
  // wrapped into a short allocation.
  constexpr size_t HALF_BITS = sizeof(size_t) * 8 / 2;
  constexpr size_t BIG = size_t(1) << HALF_BITS;
  EXPECT_TRUE(LIBC_NAMESPACE::reallocarray(ptr, BIG, BIG) == nullptr);
  ASSERT_ERRNO_EQ(ENOMEM);

  // The original block is untouched, so it is still the caller's to free.
  EXPECT_EQ(ptr[0], 42);
  ::free(ptr);
}

TEST_F(LlvmLibcReallocarrayTest, ZeroCountIsNotAnOverflow) {
  void *ptr = ::malloc(16);
  ASSERT_TRUE(ptr != nullptr);
  // Zero bytes is a legal request, and is not the overflow case.
  void *result = LIBC_NAMESPACE::reallocarray(ptr, 0, sizeof(int));
  ASSERT_ERRNO_SUCCESS();
  ::free(result);
}

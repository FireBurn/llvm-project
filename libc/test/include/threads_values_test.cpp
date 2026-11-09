//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that threads.h gives thrd_success the value 0, which programs such
/// as Mesa rely on when they compare the result of mtx_init against zero.
///
//===----------------------------------------------------------------------===//

#include "include/threads.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcThreadsValuesTest, SuccessIsZero) {
  EXPECT_EQ(static_cast<int>(thrd_success), 0);
  EXPECT_NE(static_cast<int>(thrd_busy), 0);
  EXPECT_NE(static_cast<int>(thrd_error), 0);
  EXPECT_NE(static_cast<int>(thrd_nomem), 0);
  EXPECT_NE(static_cast<int>(thrd_timedout), 0);
}

TEST(LlvmLibcThreadsValuesTest, MutexTypesAreDistinctBits) {
  EXPECT_EQ(static_cast<int>(mtx_plain), 0);
  EXPECT_EQ(static_cast<int>(mtx_recursive), 1);
  EXPECT_EQ(static_cast<int>(mtx_timed), 2);
}

//===-- Unittests for times -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/clock_t.h"
#include "hdr/types/struct_tms.h"
#include "src/sys/times/times.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcTimesTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcTimesTest, ReportsTimeSpentRunning) {
  struct tms before = {};
  const clock_t first = LIBC_NAMESPACE::times(&before);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_NE(first, static_cast<clock_t>(-1));

  // Enough work that the counter has to move.
  volatile double total = 0;
  for (long i = 0; i < 40000000; ++i)
    total += static_cast<double>(i) * 0.5;

  struct tms after = {};
  const clock_t second = LIBC_NAMESPACE::times(&after);
  ASSERT_NE(second, static_cast<clock_t>(-1));

  // Only the difference between two of these means anything: what they count
  // from is some point in the past that is the same for every call.
  EXPECT_GT(second, first);
  EXPECT_GT(after.tms_utime, before.tms_utime);
  // Nothing has been waited for, so nothing is counted against children.
  EXPECT_EQ(after.tms_cutime, static_cast<clock_t>(0));
  EXPECT_EQ(after.tms_cstime, static_cast<clock_t>(0));
}

TEST_F(LlvmLibcTimesTest, TakesNoBuffer) {
  // A caller which only wants the elapsed count need not provide anywhere to
  // put the rest.
  EXPECT_NE(LIBC_NAMESPACE::times(nullptr), static_cast<clock_t>(-1));
  ASSERT_ERRNO_SUCCESS();
}

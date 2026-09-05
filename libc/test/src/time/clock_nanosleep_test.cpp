//===-- Unittests for clock_nanosleep -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/time_macros.h"
#include "hdr/types/struct_timespec.h"
#include "src/time/clock_gettime.h"
#include "src/time/clock_nanosleep.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcClockNanosleepTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// The call reports the error rather than setting errno, which is what
// separates it from nanosleep.
TEST_F(LlvmLibcClockNanosleepTest, SleepsForTheGivenTime) {
  struct timespec before;
  ASSERT_EQ(LIBC_NAMESPACE::clock_gettime(CLOCK_MONOTONIC, &before), 0);

  struct timespec how_long = {};
  how_long.tv_nsec = 1000000; // A millisecond.
  ASSERT_EQ(
      LIBC_NAMESPACE::clock_nanosleep(CLOCK_MONOTONIC, 0, &how_long, nullptr),
      0);
  ASSERT_ERRNO_SUCCESS();

  struct timespec after;
  ASSERT_EQ(LIBC_NAMESPACE::clock_gettime(CLOCK_MONOTONIC, &after), 0);
  long long elapsed = (after.tv_sec - before.tv_sec) * 1000000000LL +
                      (after.tv_nsec - before.tv_nsec);
  ASSERT_GE(elapsed, 1000000LL);
}

// With TIMER_ABSTIME the time given is a moment rather than a length, and
// one already past returns at once.
TEST_F(LlvmLibcClockNanosleepTest, AMomentThatHasPassed) {
  struct timespec now;
  ASSERT_EQ(LIBC_NAMESPACE::clock_gettime(CLOCK_MONOTONIC, &now), 0);
  now.tv_sec -= 1;

  ASSERT_EQ(LIBC_NAMESPACE::clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,
                                            &now, nullptr),
            0);
  ASSERT_ERRNO_SUCCESS();
}

TEST_F(LlvmLibcClockNanosleepTest, ATimeThatIsNotOne) {
  struct timespec bad = {};
  bad.tv_nsec = 2000000000; // More than a second's worth of nanoseconds.
  ASSERT_EQ(LIBC_NAMESPACE::clock_nanosleep(CLOCK_MONOTONIC, 0, &bad, nullptr),
            EINVAL);
  ASSERT_ERRNO_SUCCESS();
}

TEST_F(LlvmLibcClockNanosleepTest, AClockThatCannotBeSleptOn) {
  struct timespec how_long = {};
  how_long.tv_nsec = 1;
  int result = LIBC_NAMESPACE::clock_nanosleep(-1, 0, &how_long, nullptr);
  ASSERT_TRUE(result == EINVAL || result == ENOTSUP);
  ASSERT_ERRNO_SUCCESS();
}

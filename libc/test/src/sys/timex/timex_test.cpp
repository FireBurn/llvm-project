//===-- Unittests for the clock adjustment calls --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/time_macros.h"
#include "src/__support/libc_errno.h"
#include "src/sys/timex/adjtimex.h"
#include "src/sys/timex/clock_adjtime.h"
#include "src/sys/timex/ntp_adjtime.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include <sys/timex.h>

using LlvmLibcTimexTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// Asking for nothing to be changed is a read of the clock's state, which
// needs no privilege. The answer is the clock's status rather than an error,
// and TIME_ERROR is among the values it may take.
TEST_F(LlvmLibcTimexTest, ReadingTheClockState) {
  struct timex buf = {};
  buf.modes = 0;

  int state = LIBC_NAMESPACE::adjtimex(&buf);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GE(state, 0);
  ASSERT_LE(state, TIME_ERROR);

  // The tick length is filled in, and is a hundredth of a second either way.
  ASSERT_GT(buf.tick, 0L);
}

TEST_F(LlvmLibcTimexTest, NtpAdjtimeReadsTheSameState) {
  struct timex buf = {};
  buf.modes = 0;

  int state = LIBC_NAMESPACE::ntp_adjtime(&buf);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GE(state, 0);
  ASSERT_LE(state, TIME_ERROR);
}

TEST_F(LlvmLibcTimexTest, ClockAdjtimeNamesTheClock) {
  struct timex buf = {};
  buf.modes = 0;

  int state = LIBC_NAMESPACE::clock_adjtime(CLOCK_REALTIME, &buf);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GE(state, 0);
  ASSERT_LE(state, TIME_ERROR);
}

TEST_F(LlvmLibcTimexTest, AClockThatCannotBeAdjusted) {
  struct timex buf = {};
  buf.modes = 0;
  ASSERT_EQ(LIBC_NAMESPACE::clock_adjtime(-1, &buf), -1);
  int err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == EINVAL || err == ENOTSUP || err == ENODEV);
  LIBC_NAMESPACE::libc_errno = 0;
}

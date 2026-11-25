//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for what <sys/timerfd.h> carries with it.
///
//===----------------------------------------------------------------------===//

#include "include/sys/timerfd.h"
#include "test/UnitTest/Test.h"

// Programs which include only this header use the rest of <time.h> too.
TEST(LlvmLibcSysTimerfdTest, BringsTimeH) {
  struct tm calendar = {};
  struct itimerspec timer = {};
  EXPECT_EQ(calendar.tm_sec, 0);
  EXPECT_EQ(static_cast<long>(timer.it_value.tv_nsec), 0L);
}

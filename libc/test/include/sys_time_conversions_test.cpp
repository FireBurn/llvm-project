//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for TIMEVAL_TO_TIMESPEC and TIMESPEC_TO_TIMEVAL
///
//===----------------------------------------------------------------------===//

#include "include/sys/time.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysTimeConversionsTest, ConversionsScaleTheFraction) {
  struct timeval tv = {3, 5};
  struct timespec ts;
  TIMEVAL_TO_TIMESPEC(&tv, &ts);
  EXPECT_EQ(static_cast<long long>(ts.tv_sec), static_cast<long long>(3));
  EXPECT_EQ(static_cast<long long>(ts.tv_nsec), static_cast<long long>(5000));
  ts.tv_sec = 7;
  ts.tv_nsec = 9000;
  TIMESPEC_TO_TIMEVAL(&tv, &ts);
  EXPECT_EQ(static_cast<long long>(tv.tv_sec), static_cast<long long>(7));
  EXPECT_EQ(static_cast<long long>(tv.tv_usec), static_cast<long long>(9));
}

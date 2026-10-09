//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the Linux time macros
///
//===----------------------------------------------------------------------===//

#include "include/time.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcTimeLinuxMacrosTest, ClockIdsAreTheKernelsOnLinux) {
#if defined(__linux__)
  EXPECT_EQ(CLOCK_REALTIME, 0);
  EXPECT_EQ(CLOCK_MONOTONIC, 1);
  EXPECT_EQ(CLOCK_PROCESS_CPUTIME_ID, 2);
  EXPECT_EQ(CLOCK_THREAD_CPUTIME_ID, 3);
#endif
}

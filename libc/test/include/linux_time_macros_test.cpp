//===-- Unittests for the linux/time macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the linux time macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/time-macros.h"

TEST(LlvmLibcLinuxTimeMacrosTest, AddClock_Tai) {
  EXPECT_EQ(static_cast<long long>(CLOCK_SGI_CYCLE), 10LL);
  EXPECT_EQ(static_cast<long long>(CLOCK_TAI), 11LL);
}

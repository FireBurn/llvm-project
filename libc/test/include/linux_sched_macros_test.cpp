//===-- Unittests for the linux/sched macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the linux sched macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/sched-macros.h"

TEST(LlvmLibcLinuxSchedMacrosTest, AddClone_NewtimeRlimit_NlimitsAndLine_Ma) {
  EXPECT_EQ(static_cast<long long>(CLONE_NEWTIME), 128LL);
}

TEST(LlvmLibcLinuxSchedMacrosTest, AddSched_Reset_On_Fork) {
  EXPECT_EQ(static_cast<long long>(SCHED_RESET_ON_FORK), 0x40000000LL);
}

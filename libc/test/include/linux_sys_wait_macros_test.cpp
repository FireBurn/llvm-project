//===-- Unittests for the linux/sys/wait macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the linux sys wait macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/sys-wait-macros.h"

TEST(LlvmLibcLinuxSysWaitMacrosTest, DefineTheWaitFlagsInSysWaitMacrosH) {
  EXPECT_EQ(static_cast<long long>(WNOHANG), 1LL);
  EXPECT_EQ(static_cast<long long>(WUNTRACED), 2LL);
  EXPECT_EQ(static_cast<long long>(WSTOPPED), 2LL);
  EXPECT_EQ(static_cast<long long>(WEXITED), 4LL);
  EXPECT_EQ(static_cast<long long>(WCONTINUED), 8LL);
  EXPECT_EQ(static_cast<long long>(WNOWAIT), 0x1000000LL);
  EXPECT_EQ(static_cast<long long>(__WNOTHREAD), 0x20000000LL);
  EXPECT_EQ(static_cast<long long>(__WALL), 0x40000000LL);
  EXPECT_EQ(static_cast<long long>(__WCLONE), 0x80000000LL);
  EXPECT_EQ(static_cast<long long>(P_ALL), 0LL);
  EXPECT_EQ(static_cast<long long>(P_PID), 1LL);
  EXPECT_EQ(static_cast<long long>(P_PGID), 2LL);
  EXPECT_EQ(static_cast<long long>(P_PIDFD), 3LL);
}

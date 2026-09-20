//===-- Unittests for the linux/sys/ioctl macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the linux sys ioctl macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/sys-ioctl-macros.h"

TEST(LlvmLibcLinuxSysIoctlMacrosTest, AddTheModemControlLinesToSysIoctlH) {
  EXPECT_EQ(static_cast<long long>(TIOCM_LE), 1LL);
  EXPECT_EQ(static_cast<long long>(TIOCM_DTR), 2LL);
  EXPECT_EQ(static_cast<long long>(TIOCM_RTS), 4LL);
  EXPECT_EQ(static_cast<long long>(TIOCM_ST), 8LL);
  EXPECT_EQ(static_cast<long long>(TIOCM_SR), 16LL);
  EXPECT_EQ(static_cast<long long>(TIOCM_CTS), 32LL);
  EXPECT_EQ(static_cast<long long>(TIOCM_CAR), 64LL);
  EXPECT_EQ(static_cast<long long>(TIOCM_RNG), 128LL);
  EXPECT_EQ(static_cast<long long>(TIOCM_DSR), 256LL);
  EXPECT_EQ(static_cast<long long>(TIOCM_CD), 64LL);
  EXPECT_EQ(static_cast<long long>(TIOCM_RI), 128LL);
}

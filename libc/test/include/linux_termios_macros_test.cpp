//===-- Unittests for the linux/termios macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the linux termios macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/termios-macros.h"

TEST(LlvmLibcLinuxTermiosMacrosTest, FixTcoflushAndTcioflushAndAddTheMissingT) {
  EXPECT_EQ(static_cast<long long>(CBAUDEX), 0x1000LL);
  EXPECT_EQ(static_cast<long long>(ECHOCTL), 512LL);
  EXPECT_EQ(static_cast<long long>(ECHOPRT), 1024LL);
  EXPECT_EQ(static_cast<long long>(ECHOKE), 2048LL);
  EXPECT_EQ(static_cast<long long>(FLUSHO), 0x1000LL);
  EXPECT_EQ(static_cast<long long>(PENDIN), 0x4000LL);
  EXPECT_EQ(static_cast<long long>(IEXTEN), 0x8000LL);
  EXPECT_EQ(static_cast<long long>(EXTPROC), 0x10000LL);
  EXPECT_EQ(static_cast<long long>(XCASE), 4LL);
  EXPECT_EQ(static_cast<long long>(CINTR), 3LL);
  EXPECT_EQ(static_cast<long long>(CQUIT), 28LL);
  EXPECT_EQ(static_cast<long long>(CERASE), 127LL);
  EXPECT_EQ(static_cast<long long>(CKILL), 21LL);
  EXPECT_EQ(static_cast<long long>(CEOF), 4LL);
  EXPECT_EQ(static_cast<long long>(CTIME), 0LL);
  EXPECT_EQ(static_cast<long long>(CMIN), 1LL);
  EXPECT_EQ(static_cast<long long>(CSTART), 17LL);
  EXPECT_EQ(static_cast<long long>(CSTOP), 19LL);
  EXPECT_EQ(static_cast<long long>(CSUSP), 26LL);
  EXPECT_EQ(static_cast<long long>(CEOL), 0LL);
  EXPECT_EQ(static_cast<long long>(CREPRINT), 18LL);
  EXPECT_EQ(static_cast<long long>(CDISCARD), 15LL);
  EXPECT_EQ(static_cast<long long>(CWERASE), 23LL);
  EXPECT_EQ(static_cast<long long>(CLNEXT), 22LL);
  EXPECT_EQ(static_cast<long long>(CEOL2), 0LL);
  EXPECT_EQ(static_cast<long long>(EXTA), 14LL);
  EXPECT_EQ(static_cast<long long>(EXTB), 15LL);
}

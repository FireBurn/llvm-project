//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public sys/ttydefaults.h header.
///
//===----------------------------------------------------------------------===//

#include "include/sys/ttydefaults.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysTtydefaultsTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"TTYDEF_IFLAG", static_cast<long long>(TTYDEF_IFLAG), 0x2d22LL},
      {"TTYDEF_OFLAG", static_cast<long long>(TTYDEF_OFLAG), 0x1805LL},
      {"TTYDEF_LFLAG", static_cast<long long>(TTYDEF_LFLAG), 0x8a1bLL},
      {"TTYDEF_CFLAG", static_cast<long long>(TTYDEF_CFLAG), 1440LL},
      {"TTYDEF_SPEED", static_cast<long long>(TTYDEF_SPEED), 13LL},
      {"CEOF", static_cast<long long>(CEOF), 4LL},
      {"CEOL", static_cast<long long>(CEOL), 0LL},
      {"CERASE", static_cast<long long>(CERASE), 127LL},
      {"CINTR", static_cast<long long>(CINTR), 3LL},
      {"CSTATUS", static_cast<long long>(CSTATUS), 0LL},
      {"CKILL", static_cast<long long>(CKILL), 21LL},
      {"CMIN", static_cast<long long>(CMIN), 1LL},
      {"CQUIT", static_cast<long long>(CQUIT), 28LL},
      {"CSUSP", static_cast<long long>(CSUSP), 26LL},
      {"CTIME", static_cast<long long>(CTIME), 0LL},
      {"CDSUSP", static_cast<long long>(CDSUSP), 25LL},
      {"CSTART", static_cast<long long>(CSTART), 17LL},
      {"CSTOP", static_cast<long long>(CSTOP), 19LL},
      {"CLNEXT", static_cast<long long>(CLNEXT), 22LL},
      {"CDISCARD", static_cast<long long>(CDISCARD), 15LL},
      {"CWERASE", static_cast<long long>(CWERASE), 23LL},
      {"CREPRINT", static_cast<long long>(CREPRINT), 18LL},
      {"CEOT", static_cast<long long>(CEOT), 4LL},
      {"CBRK", static_cast<long long>(CBRK), 0LL},
      {"CRPRNT", static_cast<long long>(CRPRNT), 18LL},
      {"CFLUSH", static_cast<long long>(CFLUSH), 15LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}

TEST(LlvmLibcSysTtydefaultsTest, MacrosAreDefined) {
#ifndef CTRL
  FAIL() << "CTRL is not defined";
#endif
}

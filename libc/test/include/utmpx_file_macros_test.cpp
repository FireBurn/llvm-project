//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public utmpx.h header.
///
//===----------------------------------------------------------------------===//

#include "include/utmpx.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcUtmpxFileMacrosTest, StringMacros) {
  EXPECT_STREQ(_PATH_UTMPX, "/var/run/utmp");
  EXPECT_STREQ(_PATH_WTMPX, "/var/log/wtmp");
}

TEST(LlvmLibcUtmpxFileMacrosTest, MacrosAreDefined) {
#ifndef UTMPX_FILE
  FAIL() << "UTMPX_FILE is not defined";
#endif
#ifndef UTMPX_FILENAME
  FAIL() << "UTMPX_FILENAME is not defined";
#endif
#ifndef WTMPX_FILE
  FAIL() << "WTMPX_FILE is not defined";
#endif
#ifndef WTMPX_FILENAME
  FAIL() << "WTMPX_FILENAME is not defined";
#endif
}

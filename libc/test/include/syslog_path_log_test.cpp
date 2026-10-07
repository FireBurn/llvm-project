//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public syslog.h header.
///
//===----------------------------------------------------------------------===//

#include "include/syslog.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSyslogPathLogTest, StringMacros) {
  EXPECT_STREQ(_PATH_LOG, "/dev/log");
}

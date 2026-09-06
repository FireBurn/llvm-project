//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public gshadow.h header.
///
//===----------------------------------------------------------------------===//

#include "include/gshadow.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcGshadowShadowFilesTest, StringMacros) {
  EXPECT_STREQ(_PATH_DRUM, "/dev/drum");
  EXPECT_STREQ(_PATH_DEVDB, "/var/run/dev.db");
  EXPECT_STREQ(_PATH_RWHODIR, "/var/spool/rwho");
}

TEST(LlvmLibcGshadowShadowFilesTest, MacrosAreDefined) {
#ifndef GSHADOW
  FAIL() << "GSHADOW is not defined";
#endif
}

//===-- Unittests for the stdio macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the stdio macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/stdio-macros.h"

TEST(LlvmLibcStdioMacrosTest, DefineFilename_MaxFopen_MaxL_TmpnamAndTm) {
  EXPECT_EQ(static_cast<long long>(FILENAME_MAX), 0x1000LL);
  EXPECT_EQ(static_cast<long long>(FOPEN_MAX), 16LL);
  EXPECT_EQ(static_cast<long long>(L_tmpnam), 20LL);
  EXPECT_EQ(static_cast<long long>(TMP_MAX), 0x3a2f8LL);
}

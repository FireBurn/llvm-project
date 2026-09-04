//===-- Unittests for the fnmatch macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the fnmatch macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/fnmatch-macros.h"

TEST(LlvmLibcFnmatchMacrosTest, AddTheGnuFnmatchFlags) {
  EXPECT_EQ(static_cast<long long>(FNM_FILE_NAME), 1LL);
  EXPECT_EQ(static_cast<long long>(FNM_LEADING_DIR), 8LL);
  EXPECT_EQ(static_cast<long long>(FNM_EXTMATCH), 32LL);
}

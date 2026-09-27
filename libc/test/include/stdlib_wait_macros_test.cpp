//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the wait status macros in stdlib.h
///
//===----------------------------------------------------------------------===//

#include "include/stdlib.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcStdlibWaitMacrosTest, StatusMacrosTakeApartAStatus) {
  EXPECT_TRUE(WIFEXITED(0x0300));
  EXPECT_EQ(WEXITSTATUS(0x0300), 3);
  EXPECT_TRUE(WIFSIGNALED(9));
  EXPECT_EQ(WTERMSIG(9), 9);
  EXPECT_FALSE(WIFEXITED(9));
}

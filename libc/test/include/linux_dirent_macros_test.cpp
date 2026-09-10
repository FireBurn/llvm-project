//===-- Unittests for the linux/dirent macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the linux dirent macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/dirent-macros.h"

TEST(LlvmLibcLinuxDirentMacrosTest, AddDt_WhtToTheDirectoryEntryTypes) {
  EXPECT_EQ(static_cast<long long>(DT_WHT), 14LL);
}

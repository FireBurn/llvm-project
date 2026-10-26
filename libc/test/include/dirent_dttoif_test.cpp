//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for DTTOIF and IFTODT
///
//===----------------------------------------------------------------------===//

#include "include/dirent.h"
#include "include/sys/stat.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcDirentDttoifTest, TypeBitsConvertBothWays) {
  EXPECT_EQ(static_cast<long long>(DTTOIF(DT_DIR)), static_cast<long long>(unsigned(S_IFDIR)));
  EXPECT_EQ(static_cast<long long>(DTTOIF(DT_REG)), static_cast<long long>(unsigned(S_IFREG)));
  EXPECT_EQ(static_cast<long long>(IFTODT(S_IFDIR)), static_cast<long long>(unsigned(DT_DIR)));
  EXPECT_EQ(static_cast<long long>(IFTODT(S_IFLNK | 0777)), static_cast<long long>(unsigned(DT_LNK)));
}

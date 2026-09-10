//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for struct dirent's d_fileno
///
//===----------------------------------------------------------------------===//

#include "include/dirent.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcDirentFilenoTest, FilenoIsTheInodeNumber) {
  struct dirent entry;
  entry.d_ino = 77;
  EXPECT_EQ(entry.d_fileno, entry.d_ino);
  entry.d_fileno = 78;
  EXPECT_EQ(entry.d_ino, static_cast<decltype(entry.d_ino)>(78));
}

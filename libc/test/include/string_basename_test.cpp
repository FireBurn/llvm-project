//===-- Unittests for the GNU basename in string.h ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "include/string.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcStringBasenameTest, IsDeclaredAsTheGnuFunction) {
  // The GNU form takes a const string and so cannot be the POSIX one.
  char *(*function)(const char *) noexcept = basename;
  EXPECT_TRUE(function != nullptr);
}

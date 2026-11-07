//===-- Unittests for basename, libgen.h first ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <libgen.h> redirects the name, and <string.h> leaves its own declaration out
// when that has happened.
#include "include/libgen.h"
#include "test/UnitTest/Test.h"

#include "include/string.h"

TEST(LlvmLibcLibgenFirstBasenameTest, SelectsThePosixFunction) {
  char *(*function)(char *) noexcept = basename;
  EXPECT_TRUE(function == __xpg_basename);
}

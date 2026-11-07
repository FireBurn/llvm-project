//===-- Unittests for basename, string.h first ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// As in glibc, <libgen.h> goes after <string.h>: it redirects the name.
#include "include/string.h"
#include "test/UnitTest/Test.h"

#include "include/libgen.h"

TEST(LlvmLibcLibgenBasenameTest, SelectsThePosixFunction) {
  char *(*function)(char *) noexcept = basename;
  EXPECT_TRUE(function == __xpg_basename);
}

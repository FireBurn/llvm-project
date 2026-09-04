//===-- Unittests for fopen / fclose --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/__support/CPP/scope.h"
#include "src/__support/File/file.h"
#include "src/stdio/fclose.h"
#include "src/stdio/fopen.h"
#include "src/stdio/fread.h"
#include "src/stdio/fwrite.h"
#include "src/stdio/remove.h"

#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::cpp::scope_exit;

TEST(LlvmLibcFOpenTest, PrintToFile) {
  size_t result;

  static constexpr char STRING[] = "A simple string written to a file\n";
  {
    FILE *file = LIBC_NAMESPACE::fopen("testdata/test.txt", "w");
    ASSERT_FALSE(file == nullptr);
    scope_exit close_file([&] { ASSERT_EQ(0, LIBC_NAMESPACE::fclose(file)); });

    result = LIBC_NAMESPACE::fwrite(STRING, 1, sizeof(STRING) - 1, file);
    EXPECT_GE(result, size_t(0));
  }

  {
    FILE *file = LIBC_NAMESPACE::fopen("testdata/test.txt", "r");
    ASSERT_FALSE(file == nullptr);
    scope_exit close_file([&] { ASSERT_EQ(0, LIBC_NAMESPACE::fclose(file)); });

    static char data[64] = {0};
    ASSERT_EQ(LIBC_NAMESPACE::fread(data, 1, sizeof(STRING) - 1, file),
              sizeof(STRING) - 1);
    data[sizeof(STRING) - 1] = '\0';
    ASSERT_STREQ(data, STRING);
  }
}

TEST(LlvmLibcFOpenTest, ModeStringExtensions) {
  // 'e' asks for the descriptor to be closed on exec. It may appear
  // anywhere after the first character.
  FILE *file = LIBC_NAMESPACE::fopen("testdata/mode_ext.txt", "we");
  ASSERT_TRUE(file != nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(file), 0);

  file = LIBC_NAMESPACE::fopen("testdata/mode_ext.txt", "re");
  ASSERT_TRUE(file != nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(file), 0);

  // C leaves anything past the characters it defines to the implementation,
  // and one which is not known is ignored rather than failing the open.
  file = LIBC_NAMESPACE::fopen("testdata/mode_ext.txt", "rcm");
  ASSERT_TRUE(file != nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(file), 0);

  // The first character still has to be one of the three.
  ASSERT_TRUE(LIBC_NAMESPACE::fopen("testdata/mode_ext.txt", "er") == nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::fopen("testdata/mode_ext.txt", "") == nullptr);

  // 'x' asks for the open to fail if the file is already there, so the test
  // starts from the file not being there.
  LIBC_NAMESPACE::remove("testdata/mode_excl.txt");
  file = LIBC_NAMESPACE::fopen("testdata/mode_excl.txt", "wx");
  ASSERT_TRUE(file != nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(file), 0);
  ASSERT_TRUE(LIBC_NAMESPACE::fopen("testdata/mode_excl.txt", "wx") == nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::remove("testdata/mode_excl.txt"), 0);
}

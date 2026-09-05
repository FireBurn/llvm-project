//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for fputc_unlocked and fputs_unlocked
///
//===----------------------------------------------------------------------===//

#include "src/__support/libc_errno.h"
#include "src/stdio/fclose.h"
#include "src/stdio/fgets.h"
#include "src/stdio/flockfile.h"
#include "src/stdio/fopen.h"
#include "src/stdio/fputc_unlocked.h"
#include "src/stdio/fputs_unlocked.h"
#include "src/stdio/funlockfile.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcFputcUnlockedTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcFputcUnlockedTest, WritesACharacterAndReturnsIt) {
  auto FILENAME = libc_make_test_file_path("fputc_unlocked.test");
  ::FILE *file = LIBC_NAMESPACE::fopen(FILENAME, "w");
  ASSERT_FALSE(file == nullptr);
  LIBC_NAMESPACE::flockfile(file);
  // The value written is the argument as an unsigned char.
  ASSERT_EQ(LIBC_NAMESPACE::fputc_unlocked('x', file), int('x'));
  ASSERT_EQ(LIBC_NAMESPACE::fputc_unlocked(0x1ff, file), 0xff);
  ASSERT_GE(LIBC_NAMESPACE::fputs_unlocked("yz\n", file), 0);
  LIBC_NAMESPACE::funlockfile(file);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(file), 0);

  file = LIBC_NAMESPACE::fopen(FILENAME, "r");
  ASSERT_FALSE(file == nullptr);
  char line[8] = {};
  ASSERT_TRUE(LIBC_NAMESPACE::fgets(line, sizeof(line), file) == line);
  EXPECT_EQ(line[0], 'x');
  EXPECT_EQ(static_cast<unsigned>(static_cast<unsigned char>(line[1])), 0xffu);
  EXPECT_EQ(line[2], 'y');
  EXPECT_EQ(line[3], 'z');
  ASSERT_EQ(LIBC_NAMESPACE::fclose(file), 0);
}

TEST_F(LlvmLibcFputcUnlockedTest, FailsOnAStreamOpenedForReading) {
  auto FILENAME = libc_make_test_file_path("fputc_unlocked_ro.test");
  ::FILE *file = LIBC_NAMESPACE::fopen(FILENAME, "w");
  ASSERT_FALSE(file == nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(file), 0);
  file = LIBC_NAMESPACE::fopen(FILENAME, "r");
  ASSERT_FALSE(file == nullptr);
  LIBC_NAMESPACE::flockfile(file);
  EXPECT_LT(LIBC_NAMESPACE::fputc_unlocked('x', file), 0);
  EXPECT_LT(LIBC_NAMESPACE::fputs_unlocked("x", file), 0);
  LIBC_NAMESPACE::funlockfile(file);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(file), 0);
  libc_errno = 0;
}

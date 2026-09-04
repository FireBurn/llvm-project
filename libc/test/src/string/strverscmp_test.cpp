//===-- Unittests for strverscmp ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/string/strverscmp.h"
#include "test/UnitTest/Test.h"

#define EXPECT_BEFORE(a, b) EXPECT_LT(LIBC_NAMESPACE::strverscmp(a, b), 0)
#define EXPECT_AFTER(a, b) EXPECT_GT(LIBC_NAMESPACE::strverscmp(a, b), 0)
#define EXPECT_SAME(a, b) EXPECT_EQ(LIBC_NAMESPACE::strverscmp(a, b), 0)

TEST(LlvmLibcStrVersCmpTest, EqualStrings) {
  EXPECT_SAME("", "");
  EXPECT_SAME("abc", "abc");
  EXPECT_SAME("file10", "file10");
}

TEST(LlvmLibcStrVersCmpTest, PlainText) {
  EXPECT_BEFORE("a", "b");
  EXPECT_AFTER("b", "a");
  EXPECT_BEFORE("abc", "abd");
  EXPECT_BEFORE("abc", "abcd");
  EXPECT_AFTER("abcd", "abc");
}

TEST(LlvmLibcStrVersCmpTest, NumbersReadAsNumbers) {
  // This is the whole point: as text "file10" would come before "file9".
  EXPECT_AFTER("file10", "file9");
  EXPECT_BEFORE("file9", "file10");
  EXPECT_BEFORE("file2", "file10");
  EXPECT_AFTER("file100", "file99");
  EXPECT_BEFORE("a1b2", "a2b10");
  EXPECT_BEFORE("a2b10", "a10b2");
}

TEST(LlvmLibcStrVersCmpTest, LeadingZerosAreAFraction) {
  // A run of digits which starts with a zero stands for a fraction, and a
  // fraction comes before a whole number.
  EXPECT_BEFORE("file09", "file1");
  EXPECT_AFTER("file1", "file09");
  EXPECT_BEFORE("file009", "file09");
  EXPECT_BEFORE("x0", "x1");
}

TEST(LlvmLibcStrVersCmpTest, NumbersAgainstText) {
  EXPECT_BEFORE("file", "file1");
  EXPECT_AFTER("file1", "file");
  EXPECT_BEFORE("1", "a");
  EXPECT_AFTER("a", "1");
}

TEST(LlvmLibcStrVersCmpTest, VersionNumbers) {
  EXPECT_BEFORE("1.0", "1.1");
  EXPECT_BEFORE("1.9", "1.10");
  EXPECT_BEFORE("1.2.3", "1.2.10");
  EXPECT_BEFORE("2.0", "10.0");
  EXPECT_AFTER("10.0", "9.0");
}

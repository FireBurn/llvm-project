//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unit tests for error_at_line.
///
//===----------------------------------------------------------------------===//

#include "src/error/error_at_line.h"
#include "src/error/error_message_count.h"
#include "src/error/error_one_per_line.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcErrorAtLineTest, ReportsEveryLineByDefault) {
  LIBC_NAMESPACE::error_one_per_line = 0;
  unsigned int before = LIBC_NAMESPACE::error_message_count;
  LIBC_NAMESPACE::error_at_line(0, 0, "a.c", 7, "first");
  LIBC_NAMESPACE::error_at_line(0, 0, "a.c", 7, "again");
  EXPECT_EQ(LIBC_NAMESPACE::error_message_count, before + 2);
}

// Where one message per line has been asked for, the second report of a line
// says nothing and is not counted.
TEST(LlvmLibcErrorAtLineTest, OnePerLineSuppressesTheSecond) {
  LIBC_NAMESPACE::error_one_per_line = 1;
  unsigned int before = LIBC_NAMESPACE::error_message_count;
  LIBC_NAMESPACE::error_at_line(0, 0, "b.c", 11, "first");
  LIBC_NAMESPACE::error_at_line(0, 0, "b.c", 11, "suppressed");
  EXPECT_EQ(LIBC_NAMESPACE::error_message_count, before + 1);

  // A different line is a different place, so it is reported.
  LIBC_NAMESPACE::error_at_line(0, 0, "b.c", 12, "shown");
  EXPECT_EQ(LIBC_NAMESPACE::error_message_count, before + 2);

  // So is the same line of another file.
  LIBC_NAMESPACE::error_at_line(0, 0, "c.c", 12, "shown");
  EXPECT_EQ(LIBC_NAMESPACE::error_message_count, before + 3);
  LIBC_NAMESPACE::error_one_per_line = 0;
}

// Without a file name there is no place to leave out of the message.
TEST(LlvmLibcErrorAtLineTest, NullFileName) {
  LIBC_NAMESPACE::error_one_per_line = 0;
  unsigned int before = LIBC_NAMESPACE::error_message_count;
  LIBC_NAMESPACE::error_at_line(0, 0, nullptr, 3, "no file");
  EXPECT_EQ(LIBC_NAMESPACE::error_message_count, before + 1);
}

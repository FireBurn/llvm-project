//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for setlinebuf
///
//===----------------------------------------------------------------------===//

#include "src/stdio/fclose.h"
#include "src/stdio/fflush.h"
#include "src/stdio/fopen.h"
#include "src/stdio/fread.h"
#include "src/stdio/fwrite.h"
#include "src/stdio/setlinebuf.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcSetlinebufTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// A line buffered stream reaches the file when a newline is written, without
// a flush; one that is fully buffered does not until the buffer fills.
TEST_F(LlvmLibcSetlinebufTest, WritesALineAtANewline) {
  auto FILENAME = libc_make_test_file_path("setlinebuf.test");
  ::FILE *writer = LIBC_NAMESPACE::fopen(FILENAME, "w");
  ASSERT_FALSE(writer == nullptr);
  LIBC_NAMESPACE::setlinebuf(writer);
  ASSERT_EQ(LIBC_NAMESPACE::fwrite("ab", 1, 2, writer), size_t(2));

  ::FILE *reader = LIBC_NAMESPACE::fopen(FILENAME, "r");
  ASSERT_FALSE(reader == nullptr);
  char data[8] = {};
  EXPECT_EQ(LIBC_NAMESPACE::fread(data, 1, sizeof(data), reader), size_t(0));

  ASSERT_EQ(LIBC_NAMESPACE::fwrite("c\n", 1, 2, writer), size_t(2));
  LIBC_NAMESPACE::fclose(reader);
  reader = LIBC_NAMESPACE::fopen(FILENAME, "r");
  ASSERT_FALSE(reader == nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::fread(data, 1, sizeof(data), reader), size_t(4));
  EXPECT_EQ(data[0], 'a');
  EXPECT_EQ(data[2], 'c');
  EXPECT_EQ(data[3], '\n');
  ASSERT_EQ(LIBC_NAMESPACE::fclose(reader), 0);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(writer), 0);
}

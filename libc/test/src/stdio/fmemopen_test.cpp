//===-- Unittests for the fmemopen function -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/fclose.h"
#include "src/stdio/feof.h"
#include "src/stdio/fflush.h"
#include "src/stdio/fmemopen.h"
#include "src/stdio/fread.h"
#include "src/stdio/fseek.h"
#include "src/stdio/ftell.h"
#include "src/stdio/fwrite.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

#include "hdr/stdio_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/libc_errno.h"

using LlvmLibcFMemOpenTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcFMemOpenTest, ReadWholeBuffer) {
  char data[] = "hello world";
  ::FILE *f = LIBC_NAMESPACE::fmemopen(data, sizeof(data) - 1, "r");
  ASSERT_TRUE(f != nullptr);

  char got[16] = {};
  ASSERT_EQ(LIBC_NAMESPACE::fread(got, 1, sizeof(got), f), size_t(11));
  ASSERT_STREQ(got, "hello world");
  ASSERT_NE(LIBC_NAMESPACE::feof(f), 0);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
}

TEST_F(LlvmLibcFMemOpenTest, ReadStopsAtTheSizeGiven) {
  char data[] = "hello world";
  ::FILE *f = LIBC_NAMESPACE::fmemopen(data, 5, "r");
  ASSERT_TRUE(f != nullptr);

  char got[16] = {};
  ASSERT_EQ(LIBC_NAMESPACE::fread(got, 1, sizeof(got), f), size_t(5));
  ASSERT_STREQ(got, "hello");
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
}

TEST_F(LlvmLibcFMemOpenTest, WriteKeepsRoomForTheTerminator) {
  // A stream open for writing holds back the last byte of the block for a
  // null terminator, so eight bytes of block take seven bytes of data.
  char block[8];
  for (size_t i = 0; i < sizeof(block); ++i)
    block[i] = 'x';

  ::FILE *f = LIBC_NAMESPACE::fmemopen(block, sizeof(block), "w");
  ASSERT_TRUE(f != nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::fwrite("12345678", 1, 8, f), size_t(8));
  ASSERT_EQ(LIBC_NAMESPACE::fflush(f), 0);
  ASSERT_STREQ(block, "1234567");
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
}

TEST_F(LlvmLibcFMemOpenTest, WriteOverflowIsDroppedNotReported) {
  char block[4];
  ::FILE *f = LIBC_NAMESPACE::fmemopen(block, sizeof(block), "w");
  ASSERT_TRUE(f != nullptr);
  // What does not fit is dropped, and the count still says it was all taken.
  ASSERT_EQ(LIBC_NAMESPACE::fwrite("0123456789", 1, 10, f), size_t(10));
  ASSERT_EQ(LIBC_NAMESPACE::fflush(f), 0);
  ASSERT_STREQ(block, "012");
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
}

TEST_F(LlvmLibcFMemOpenTest, SeekAndTell) {
  char data[] = "0123456789";
  ::FILE *f = LIBC_NAMESPACE::fmemopen(data, 10, "r");
  ASSERT_TRUE(f != nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::fseek(f, 4, SEEK_SET), 0);
  ASSERT_EQ(LIBC_NAMESPACE::ftell(f), long(4));
  char got[4] = {};
  ASSERT_EQ(LIBC_NAMESPACE::fread(got, 1, 3, f), size_t(3));
  ASSERT_STREQ(got, "456");

  ASSERT_EQ(LIBC_NAMESPACE::fseek(f, 0, SEEK_END), 0);
  ASSERT_EQ(LIBC_NAMESPACE::ftell(f), long(10));
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
}

TEST_F(LlvmLibcFMemOpenTest, AppendStartsAtTheTerminator) {
  char block[16] = "abc";
  ::FILE *f = LIBC_NAMESPACE::fmemopen(block, sizeof(block), "a");
  ASSERT_TRUE(f != nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::ftell(f), long(3));
  ASSERT_EQ(LIBC_NAMESPACE::fwrite("def", 1, 3, f), size_t(3));
  ASSERT_EQ(LIBC_NAMESPACE::fflush(f), 0);
  ASSERT_STREQ(block, "abcdef");
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
}

TEST_F(LlvmLibcFMemOpenTest, OwnBuffer) {
  ::FILE *f = LIBC_NAMESPACE::fmemopen(nullptr, 64, "w+");
  ASSERT_TRUE(f != nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::fwrite("written", 1, 7, f), size_t(7));
  ASSERT_EQ(LIBC_NAMESPACE::fseek(f, 0, SEEK_SET), 0);
  char got[16] = {};
  ASSERT_EQ(LIBC_NAMESPACE::fread(got, 1, 7, f), size_t(7));
  ASSERT_STREQ(got, "written");
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
}

TEST_F(LlvmLibcFMemOpenTest, ZeroSizeIsAnEmptyStream) {
  char block[1];
  ::FILE *f = LIBC_NAMESPACE::fmemopen(block, 0, "r");
  ASSERT_TRUE(f != nullptr);
  char got[4] = {};
  ASSERT_EQ(LIBC_NAMESPACE::fread(got, 1, sizeof(got), f), size_t(0));
  ASSERT_NE(LIBC_NAMESPACE::feof(f), 0);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
}

TEST_F(LlvmLibcFMemOpenTest, BadModeFails) {
  char block[8];
  ASSERT_TRUE(LIBC_NAMESPACE::fmemopen(block, sizeof(block), nullptr) ==
              nullptr);
  ASSERT_ERRNO_EQ(EINVAL);
}

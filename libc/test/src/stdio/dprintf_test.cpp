//===-- Unittests for dprintf and vdprintf --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/libc_errno.h"
#include "src/fcntl/open.h"
#include "src/stdio/dprintf.h"
#include "src/stdio/remove.h"
#include "src/stdio/vdprintf.h"
#include "src/unistd/close.h"
#include "src/unistd/read.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

#include <stdarg.h>

using LlvmLibcDprintfTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

namespace {

constexpr const char *FILENAME = "dprintf.test";

int call_vdprintf(int fd, const char *format, ...) {
  va_list vlist;
  va_start(vlist, format);
  int ret = LIBC_NAMESPACE::vdprintf(fd, format, vlist);
  va_end(vlist);
  return ret;
}

// Reads back everything written to the test file.
size_t read_back(char *buf, size_t buflen) {
  int fd = LIBC_NAMESPACE::open(FILENAME, O_RDONLY);
  if (fd < 0)
    return 0;
  ssize_t got = LIBC_NAMESPACE::read(fd, buf, buflen - 1);
  LIBC_NAMESPACE::close(fd);
  if (got < 0)
    return 0;
  buf[got] = '\0';
  return static_cast<size_t>(got);
}

} // anonymous namespace

TEST_F(LlvmLibcDprintfTest, WritesToTheDescriptor) {
  int fd =
      LIBC_NAMESPACE::open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);

  constexpr const char *EXPECTED = "answer=42 3.14 x\n";
  constexpr size_t EXPECTED_LEN = sizeof("answer=42 3.14 x\n") - 1;
  int written = LIBC_NAMESPACE::dprintf(fd, "%s=%d %.2f %c\n", "answer", 42,
                                        3.14159, 'x');
  // The count reported is what was written, not what the format was.
  EXPECT_EQ(written, static_cast<int>(EXPECTED_LEN));
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);

  char buf[64] = {};
  ASSERT_EQ(read_back(buf, sizeof(buf)), EXPECTED_LEN);
  EXPECT_TRUE(string_view(buf) == EXPECTED);
  ASSERT_EQ(LIBC_NAMESPACE::remove(FILENAME), 0);
}

TEST_F(LlvmLibcDprintfTest, VarargsForm) {
  int fd =
      LIBC_NAMESPACE::open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  EXPECT_EQ(call_vdprintf(fd, "%d-%d", 1, 2), 3);
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);

  char buf[32] = {};
  ASSERT_EQ(read_back(buf, sizeof(buf)), size_t(3));
  EXPECT_TRUE(string_view(buf) == "1-2");
  ASSERT_EQ(LIBC_NAMESPACE::remove(FILENAME), 0);
}

TEST_F(LlvmLibcDprintfTest, MoreThanTheBufferHolds) {
  int fd =
      LIBC_NAMESPACE::open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);

  // The internal buffer is flushed as it fills, so a run longer than it can
  // hold still comes out whole and in order.
  int total = 0;
  for (int i = 0; i < 500; ++i)
    total += LIBC_NAMESPACE::dprintf(fd, "%04d", i);
  EXPECT_EQ(total, 2000);
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);

  char buf[4096] = {};
  ASSERT_EQ(read_back(buf, sizeof(buf)), size_t(2000));
  EXPECT_TRUE(string_view(buf).starts_with("000000010002"));
  EXPECT_TRUE(string_view(buf).ends_with("049704980499"));
  ASSERT_EQ(LIBC_NAMESPACE::remove(FILENAME), 0);
}

TEST_F(LlvmLibcDprintfTest, ABadDescriptorIsReported) {
  EXPECT_EQ(LIBC_NAMESPACE::dprintf(-1, "anything"), -1);
  ASSERT_ERRNO_FAILURE();
}

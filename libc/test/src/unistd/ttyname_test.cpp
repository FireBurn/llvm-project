//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for ttyname and ttyname_r.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/libc_errno.h"
#include "src/fcntl/open.h"
#include "src/unistd/close.h"
#include "src/unistd/isatty.h"
#include "src/unistd/pipe.h"
#include "src/unistd/ttyname.h"
#include "src/unistd/ttyname_r.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcTtynameTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

TEST_F(LlvmLibcTtynameTest, APipeIsNotATerminal) {
  int fds[2];
  ASSERT_EQ(LIBC_NAMESPACE::pipe(fds), 0);

  char buf[64];
  EXPECT_EQ(LIBC_NAMESPACE::ttyname_r(fds[0], buf, sizeof(buf)), ENOTTY);
  EXPECT_TRUE(LIBC_NAMESPACE::ttyname(fds[0]) == nullptr);
  ASSERT_ERRNO_EQ(ENOTTY);

  ASSERT_EQ(LIBC_NAMESPACE::close(fds[0]), 0);
  ASSERT_EQ(LIBC_NAMESPACE::close(fds[1]), 0);
}

TEST_F(LlvmLibcTtynameTest, ClosedDescriptorFails) {
  char buf[64];
  EXPECT_EQ(LIBC_NAMESPACE::ttyname_r(-1, buf, sizeof(buf)), EBADF);
}

TEST_F(LlvmLibcTtynameTest, NullBufferFails) {
  EXPECT_EQ(LIBC_NAMESPACE::ttyname_r(0, nullptr, 64), EINVAL);
}

TEST_F(LlvmLibcTtynameTest, ACharacterDeviceIsNotEnough) {
  int fd = LIBC_NAMESPACE::open("/dev/null", O_RDONLY);
  if (fd < 0)
    return;

  // /dev/null is a character device but not a terminal, so it has no
  // terminal name.
  char buf[64];
  // ttyname_r reports the failure in its return value and leaves errno
  // alone, which is what separates it from ttyname.
  EXPECT_EQ(LIBC_NAMESPACE::ttyname_r(fd, buf, sizeof(buf)), ENOTTY);
  ASSERT_ERRNO_SUCCESS();

  EXPECT_EQ(LIBC_NAMESPACE::isatty(fd), 0);
  ASSERT_ERRNO_EQ(ENOTTY);

  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);
}

TEST_F(LlvmLibcTtynameTest, NamesATerminal) {
  // The test may run without a controlling terminal, so the pty multiplexer
  // stands in for one. It is a terminal in its own right.
  int fd = LIBC_NAMESPACE::open("/dev/ptmx", O_RDWR);
  if (fd < 0)
    return;
  ASSERT_EQ(LIBC_NAMESPACE::isatty(fd), 1);

  char buf[64];
  EXPECT_EQ(LIBC_NAMESPACE::ttyname_r(fd, buf, sizeof(buf)), 0);
  EXPECT_TRUE(string_view(buf) == "/dev/ptmx");

  char *name = LIBC_NAMESPACE::ttyname(fd);
  ASSERT_TRUE(name != nullptr);
  EXPECT_TRUE(string_view(name) == "/dev/ptmx");

  // A buffer too small for the name is reported rather than truncated.
  char small[4];
  EXPECT_EQ(LIBC_NAMESPACE::ttyname_r(fd, small, sizeof(small)), ERANGE);

  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);
}

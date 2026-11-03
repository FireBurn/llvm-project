//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the tests of the pidfd functions.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "src/sys/pidfd/pidfd_getfd.h"
#include "src/sys/pidfd/pidfd_open.h"
#include "src/sys/pidfd/pidfd_send_signal.h"
#include "src/unistd/close.h"
#include "src/unistd/getpid.h"
#include "src/unistd/pipe.h"
#include "src/unistd/read.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;
using LlvmLibcPidfdTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcPidfdTest, OpenSignalAndGetAnFd) {
  int pidfd = LIBC_NAMESPACE::pidfd_open(LIBC_NAMESPACE::getpid(), 0);
  ASSERT_GE(pidfd, 0);

  // Signal 0 only checks that the process can be signalled.
  ASSERT_THAT(LIBC_NAMESPACE::pidfd_send_signal(pidfd, 0, nullptr, 0),
              Succeeds(0));

  // A copy of the write end of a pipe works as the original does.
  int fds[2];
  ASSERT_EQ(LIBC_NAMESPACE::pipe(fds), 0);
  int copy = LIBC_NAMESPACE::pidfd_getfd(pidfd, fds[1], 0);
  ASSERT_GE(copy, 0);
  ASSERT_NE(copy, fds[1]);
  ASSERT_EQ(LIBC_NAMESPACE::write(copy, "x", 1), ssize_t(1));
  char byte = 0;
  ASSERT_EQ(LIBC_NAMESPACE::read(fds[0], &byte, 1), ssize_t(1));
  ASSERT_EQ(byte, 'x');

  ASSERT_EQ(LIBC_NAMESPACE::close(copy), 0);
  ASSERT_EQ(LIBC_NAMESPACE::close(fds[0]), 0);
  ASSERT_EQ(LIBC_NAMESPACE::close(fds[1]), 0);
  ASSERT_EQ(LIBC_NAMESPACE::close(pidfd), 0);
}

TEST_F(LlvmLibcPidfdTest, ReportsErrors) {
  // No process has an id this large.
  ASSERT_THAT(LIBC_NAMESPACE::pidfd_open(0x7ffffff0, 0), Fails(ESRCH));
  // Flags the kernel does not know are refused.
  ASSERT_THAT(LIBC_NAMESPACE::pidfd_open(LIBC_NAMESPACE::getpid(), 0x80000000u),
              Fails(EINVAL));
  ASSERT_THAT(LIBC_NAMESPACE::pidfd_send_signal(-1, 0, nullptr, 0),
              Fails(EBADF));
}

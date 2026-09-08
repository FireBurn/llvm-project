//===-- Unittests for flock -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_file_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/fcntl/open.h"
#include "src/stdio/remove.h"
#include "src/sys/file/flock.h"
#include "src/unistd/close.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcFlockTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcFlockTest, TakeAndRelease) {
  constexpr const char *FILENAME = "flock.test";
  int fd =
      LIBC_NAMESPACE::open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);

  ASSERT_THAT(LIBC_NAMESPACE::flock(fd, LOCK_EX), Succeeds(0));
  // Taking it again through the same description just keeps it.
  ASSERT_THAT(LIBC_NAMESPACE::flock(fd, LOCK_EX), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::flock(fd, LOCK_UN), Succeeds(0));

  // A shared lock, which others may also hold.
  ASSERT_THAT(LIBC_NAMESPACE::flock(fd, LOCK_SH), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::flock(fd, LOCK_UN), Succeeds(0));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_EQ(LIBC_NAMESPACE::remove(FILENAME), 0);
}

TEST_F(LlvmLibcFlockTest, ATakenLockIsReported) {
  constexpr const char *FILENAME = "flock.contended.test";
  int first =
      LIBC_NAMESPACE::open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(first, 0);
  // A separate open is a separate description, so its lock is a separate
  // claim on the same file.
  int second = LIBC_NAMESPACE::open(FILENAME, O_RDONLY);
  ASSERT_GT(second, 0);

  ASSERT_THAT(LIBC_NAMESPACE::flock(first, LOCK_EX), Succeeds(0));
  // Asking for it without waiting says it is taken rather than blocking.
  EXPECT_THAT(LIBC_NAMESPACE::flock(second, LOCK_EX | LOCK_NB),
              Fails(EWOULDBLOCK, -1));

  ASSERT_THAT(LIBC_NAMESPACE::flock(first, LOCK_UN), Succeeds(0));
  EXPECT_THAT(LIBC_NAMESPACE::flock(second, LOCK_EX | LOCK_NB), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::flock(second, LOCK_UN), Succeeds(0));

  ASSERT_THAT(LIBC_NAMESPACE::close(first), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(second), Succeeds(0));
  ASSERT_EQ(LIBC_NAMESPACE::remove(FILENAME), 0);
}

TEST_F(LlvmLibcFlockTest, BadDescriptorAndOperation) {
  EXPECT_THAT(LIBC_NAMESPACE::flock(-1, LOCK_EX), Fails(EBADF, -1));

  constexpr const char *FILENAME = "flock.bad.test";
  int fd =
      LIBC_NAMESPACE::open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  // An operation which is none of the four is not one the kernel knows.
  EXPECT_THAT(LIBC_NAMESPACE::flock(fd, 0), Fails(EINVAL, -1));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_EQ(LIBC_NAMESPACE::remove(FILENAME), 0);
}

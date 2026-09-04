//===-- Unittests for fstatat ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_stat.h"
#include "src/fcntl/open.h"
#include "src/stdio/remove.h"
#include "src/sys/stat/fstatat.h"
#include "src/unistd/close.h"
#include "src/unistd/symlink.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcFstatatTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcFstatatTest, RelativeToTheWorkingDirectory) {
  constexpr const char *FILENAME = "fstatat.test";
  int fd =
      LIBC_NAMESPACE::open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  struct stat buf;
  ASSERT_THAT(LIBC_NAMESPACE::fstatat(AT_FDCWD, FILENAME, &buf, 0),
              Succeeds(0));
  EXPECT_TRUE(S_ISREG(buf.st_mode));
  EXPECT_EQ(buf.st_size, off_t(0));

  ASSERT_EQ(LIBC_NAMESPACE::remove(FILENAME), 0);
}

TEST_F(LlvmLibcFstatatTest, RelativeToADirectory) {
  constexpr const char *FILENAME = "fstatat.dirfd.test";
  int fd =
      LIBC_NAMESPACE::open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  int dirfd = LIBC_NAMESPACE::open(".", O_RDONLY | O_DIRECTORY);
  ASSERT_GT(dirfd, 0);

  struct stat buf;
  ASSERT_THAT(LIBC_NAMESPACE::fstatat(dirfd, FILENAME, &buf, 0), Succeeds(0));
  EXPECT_TRUE(S_ISREG(buf.st_mode));

  ASSERT_THAT(LIBC_NAMESPACE::close(dirfd), Succeeds(0));
  ASSERT_EQ(LIBC_NAMESPACE::remove(FILENAME), 0);
}

TEST_F(LlvmLibcFstatatTest, NotFollowingALink) {
  constexpr const char *TARGET = "fstatat.target.test";
  constexpr const char *LINK = "fstatat.link.test";
  int fd = LIBC_NAMESPACE::open(TARGET, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  LIBC_NAMESPACE::unlink(LINK);
  libc_errno = 0;
  ASSERT_THAT(LIBC_NAMESPACE::symlink(TARGET, LINK), Succeeds(0));

  struct stat followed;
  ASSERT_THAT(LIBC_NAMESPACE::fstatat(AT_FDCWD, LINK, &followed, 0),
              Succeeds(0));
  EXPECT_TRUE(S_ISREG(followed.st_mode));

  struct stat itself;
  ASSERT_THAT(
      LIBC_NAMESPACE::fstatat(AT_FDCWD, LINK, &itself, AT_SYMLINK_NOFOLLOW),
      Succeeds(0));
  EXPECT_TRUE(S_ISLNK(itself.st_mode));

  ASSERT_EQ(LIBC_NAMESPACE::remove(LINK), 0);
  ASSERT_EQ(LIBC_NAMESPACE::remove(TARGET), 0);
}

TEST_F(LlvmLibcFstatatTest, MissingPath) {
  struct stat buf;
  EXPECT_THAT(LIBC_NAMESPACE::fstatat(AT_FDCWD, "fstatat.no.such", &buf, 0),
              Fails(ENOENT, -1));
}

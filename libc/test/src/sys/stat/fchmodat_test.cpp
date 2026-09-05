//===-- Unittests for fchmodat --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_stat.h"
#include "src/__support/libc_errno.h"
#include "src/fcntl/open.h"
#include "src/sys/stat/fchmodat.h"
#include "src/sys/stat/fstat.h"
#include "src/sys/stat/stat.h"
#include "src/unistd/close.h"
#include "src/unistd/symlink.h"
#include "src/unistd/unlink.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcFchmodatTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcFchmodatTest, ChangeAndOpen) {
  // The test file is initially writable. We open it for writing and ensure
  // that it indeed can be opened for writing. Next, we close the file and
  // make it readonly using chmod. We test that chmod actually succeeded by
  // trying to open the file for writing and failing.
  constexpr const char *TEST_FILE = "testdata/fchmodat.test";
  constexpr const char *TEST_DIR = "testdata";
  constexpr const char *TEST_FILE_BASENAME = "fchmodat.test";
  const char WRITE_DATA[] = "fchmodat test";
  constexpr ssize_t WRITE_SIZE = ssize_t(sizeof(WRITE_DATA));

  int fd = LIBC_NAMESPACE::open(TEST_FILE, O_CREAT | O_WRONLY, S_IRWXU);
  ASSERT_GT(fd, 0);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_EQ(LIBC_NAMESPACE::write(fd, WRITE_DATA, sizeof(WRITE_DATA)),
            WRITE_SIZE);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  int dirfd = LIBC_NAMESPACE::open(TEST_DIR, O_DIRECTORY);
  ASSERT_GT(dirfd, 0);
  ASSERT_ERRNO_SUCCESS();

  EXPECT_THAT(LIBC_NAMESPACE::fchmodat(dirfd, TEST_FILE_BASENAME, S_IRUSR, 0),
              Succeeds(0));

  // Opening for writing should fail.
  EXPECT_EQ(LIBC_NAMESPACE::open(TEST_FILE, O_APPEND | O_WRONLY), -1);
  ASSERT_ERRNO_FAILURE();
  // But opening for reading should succeed.
  fd = LIBC_NAMESPACE::open(TEST_FILE, O_APPEND | O_RDONLY);
  EXPECT_GT(fd, 0);
  ASSERT_ERRNO_SUCCESS();

  EXPECT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  EXPECT_THAT(LIBC_NAMESPACE::fchmodat(dirfd, TEST_FILE_BASENAME, S_IRWXU, 0),
              Succeeds(0));

  EXPECT_THAT(LIBC_NAMESPACE::close(dirfd), Succeeds(0));
}

TEST_F(LlvmLibcFchmodatTest, NonExistentFile) {
  ASSERT_THAT(
      LIBC_NAMESPACE::fchmodat(AT_FDCWD, "non-existent-file", S_IRUSR, 0),
      Fails(ENOENT));
}

TEST_F(LlvmLibcFchmodatTest, EmptyPathActsOnTheDescriptor) {
  constexpr const char *FILENAME = "fchmodat_empty_path.test";
  auto TEST_FILE = libc_make_test_file_path(FILENAME);
  int fd = LIBC_NAMESPACE::open(TEST_FILE, O_WRONLY | O_CREAT, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  // AT_EMPTY_PATH names what the descriptor is open on, so the path is empty
  // rather than naming anything.
  int path_fd = LIBC_NAMESPACE::open(TEST_FILE, O_PATH);
  ASSERT_GT(path_fd, 0);
  EXPECT_THAT(
      LIBC_NAMESPACE::fchmodat(path_fd, "", S_IRUSR | S_IWUSR, AT_EMPTY_PATH),
      Succeeds(0));

  struct stat statbuf;
  ASSERT_EQ(LIBC_NAMESPACE::fstat(path_fd, &statbuf), 0);
  EXPECT_EQ(statbuf.st_mode & 07777, mode_t(S_IRUSR | S_IWUSR));

  ASSERT_THAT(LIBC_NAMESPACE::close(path_fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(TEST_FILE), Succeeds(0));
}

TEST_F(LlvmLibcFchmodatTest, DoesNotFollowALinkWhenToldNotTo) {
  constexpr const char *TARGET_NAME = "fchmodat_nofollow_target.test";
  constexpr const char *LINK_NAME = "fchmodat_nofollow_link.test";
  auto TARGET = libc_make_test_file_path(TARGET_NAME);
  auto LINK = libc_make_test_file_path(LINK_NAME);

  int fd = LIBC_NAMESPACE::open(TARGET, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::symlink(TARGET, LINK), Succeeds(0));

  // A mode cannot be set on a symbolic link at all, so this reports that
  // rather than quietly changing what the link points at.
  ASSERT_EQ(
      LIBC_NAMESPACE::fchmodat(AT_FDCWD, LINK, S_IRWXU, AT_SYMLINK_NOFOLLOW),
      -1);
  // That failure was the point of the call, so what it left behind is not an
  // error the rest of the test should be checked against.
  libc_errno = 0;

  struct stat statbuf;
  ASSERT_EQ(LIBC_NAMESPACE::stat(TARGET, &statbuf), 0);
  EXPECT_EQ(statbuf.st_mode & 07777, mode_t(S_IRUSR | S_IWUSR));

  ASSERT_THAT(LIBC_NAMESPACE::unlink(LINK), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(TARGET), Succeeds(0));
}

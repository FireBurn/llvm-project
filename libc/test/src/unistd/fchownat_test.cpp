//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for fchownat.
///
//===----------------------------------------------------------------------===//

#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/__support/CPP/scope.h"
#include "src/fcntl/open.h"
#include "src/unistd/close.h"
#include "src/unistd/fchownat.h"
#include "src/unistd/getgid.h"
#include "src/unistd/getuid.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcFchownatTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcFchownatTest, FchownatSuccessAtFdCwd) {
  uid_t my_uid = LIBC_NAMESPACE::getuid();
  gid_t my_gid = LIBC_NAMESPACE::getgid();
  constexpr const char *FILENAME = "fchownat_at_fdcwd.test";
  auto TEST_FILE = libc_make_test_file_path(FILENAME);

  int write_fd = LIBC_NAMESPACE::open(TEST_FILE, O_WRONLY | O_CREAT, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(write_fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(write_fd), Succeeds(0));

  LIBC_NAMESPACE::cpp::scope_exit cleanup(
      [&] { EXPECT_THAT(LIBC_NAMESPACE::unlink(TEST_FILE), Succeeds(0)); });

  // Change ownership to current user/group with AT_FDCWD.
  ASSERT_THAT(LIBC_NAMESPACE::fchownat(AT_FDCWD, TEST_FILE, my_uid, my_gid, 0),
              Succeeds(0));

  // Calling with AT_SYMLINK_NOFOLLOW on a regular file succeeds.
  ASSERT_THAT(LIBC_NAMESPACE::fchownat(AT_FDCWD, TEST_FILE, my_uid, my_gid,
                                       AT_SYMLINK_NOFOLLOW),
              Succeeds(0));

  // Passing -1 for owner and group leaves them unchanged without error.
  ASSERT_THAT(LIBC_NAMESPACE::fchownat(AT_FDCWD, TEST_FILE,
                                       static_cast<uid_t>(-1),
                                       static_cast<gid_t>(-1), 0),
              Succeeds(0));
}

TEST_F(LlvmLibcFchownatTest, FchownatSuccessWithDirFd) {
  uid_t my_uid = LIBC_NAMESPACE::getuid();
  gid_t my_gid = LIBC_NAMESPACE::getgid();
  auto TEST_DIR = libc_make_test_file_path("testdata");
  constexpr const char *TEST_FILE_BASENAME = "fchownat_dir.test";
  auto TEST_FILE_PATH = libc_make_test_file_path("testdata/fchownat_dir.test");

  int write_fd =
      LIBC_NAMESPACE::open(TEST_FILE_PATH, O_WRONLY | O_CREAT, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(write_fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(write_fd), Succeeds(0));

  LIBC_NAMESPACE::cpp::scope_exit cleanup_file([&] {
    EXPECT_THAT(LIBC_NAMESPACE::unlink(TEST_FILE_PATH), Succeeds(0));
  });

  int dirfd = LIBC_NAMESPACE::open(TEST_DIR, O_DIRECTORY | O_RDONLY);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(dirfd, 0);

  LIBC_NAMESPACE::cpp::scope_exit cleanup_dir(
      [&] { EXPECT_THAT(LIBC_NAMESPACE::close(dirfd), Succeeds(0)); });

  // Change ownership through directory file descriptor and relative basename.
  ASSERT_THAT(
      LIBC_NAMESPACE::fchownat(dirfd, TEST_FILE_BASENAME, my_uid, my_gid, 0),
      Succeeds(0));
}

TEST_F(LlvmLibcFchownatTest, FchownatNonExistentFile) {
  auto BAD_PATH = libc_make_test_file_path("non_existent_file_fchownat.test");
  ASSERT_THAT(LIBC_NAMESPACE::fchownat(AT_FDCWD, BAD_PATH, 1000, 1000, 0),
              Fails(ENOENT));
}

TEST_F(LlvmLibcFchownatTest, FchownatInvalidDirFd) {
  ASSERT_THAT(LIBC_NAMESPACE::fchownat(-1, "relative_path_fchownat.test", 1000,
                                       1000, 0),
              Fails(EBADF));
}

TEST_F(LlvmLibcFchownatTest, FchownatInvalidFlags) {
  uid_t my_uid = LIBC_NAMESPACE::getuid();
  gid_t my_gid = LIBC_NAMESPACE::getgid();
  constexpr const char *FILENAME = "fchownat_invalid_flags.test";
  auto TEST_FILE = libc_make_test_file_path(FILENAME);

  int write_fd = LIBC_NAMESPACE::open(TEST_FILE, O_WRONLY | O_CREAT, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(write_fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(write_fd), Succeeds(0));

  LIBC_NAMESPACE::cpp::scope_exit cleanup(
      [&] { EXPECT_THAT(LIBC_NAMESPACE::unlink(TEST_FILE), Succeeds(0)); });

  // Passing invalid flags should fail with EINVAL.
  ASSERT_THAT(LIBC_NAMESPACE::fchownat(AT_FDCWD, TEST_FILE, my_uid, my_gid, ~0),
              Fails(EINVAL));
}
//===-- Unittests for fchownat --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/fcntl/open.h"
#include "src/sys/stat/stat.h"
#include "src/unistd/close.h"
#include "src/unistd/fchownat.h"
#include "src/unistd/getegid.h"
#include "src/unistd/geteuid.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcFchownatTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// Asking for the owner already held is allowed whoever is asking, which is
// the one case a test that is not root can make.
TEST_F(LlvmLibcFchownatTest, RelativeToTheWorkingDirectory) {
  auto path = libc_make_test_file_path("fchownat.test");
  int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  uid_t uid = LIBC_NAMESPACE::geteuid();
  gid_t gid = LIBC_NAMESPACE::getegid();
  ASSERT_THAT(LIBC_NAMESPACE::fchownat(AT_FDCWD, path, uid, gid, 0),
              Succeeds(0));

  struct stat st;
  ASSERT_THAT(LIBC_NAMESPACE::stat(path, &st), Succeeds(0));
  ASSERT_EQ(st.st_uid, uid);
  ASSERT_EQ(st.st_gid, gid);

  // A value of -1 for either leaves that one as it was.
  ASSERT_THAT(LIBC_NAMESPACE::fchownat(AT_FDCWD, path, static_cast<uid_t>(-1),
                                       static_cast<gid_t>(-1), 0),
              Succeeds(0));

  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcFchownatTest, PathThatIsNotThere) {
  ASSERT_THAT(LIBC_NAMESPACE::fchownat(AT_FDCWD, "llvm-libc-not-here", 0, 0, 0),
              Fails(ENOENT));
}

TEST_F(LlvmLibcFchownatTest, DirectoryDescriptorThatIsNotOne) {
  ASSERT_THAT(LIBC_NAMESPACE::fchownat(-1, "llvm-libc-not-here", 0, 0, 0),
              Fails(EBADF));
}

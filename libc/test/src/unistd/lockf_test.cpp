//===-- Unittests for lockf -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/fcntl_macros.h"
#include "src/fcntl/open.h"
#include "src/unistd/close.h"
#include "src/unistd/lockf.h"
#include "src/unistd/lseek.h"
#include "src/unistd/unlink.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include "hdr/stdio_macros.h"
#include "hdr/unistd_macros.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcLockfTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {

int open_with_content(const char *path) {
  constexpr char TEXT[] = "lockf test file";
  int fd = LIBC_NAMESPACE::open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
  EXPECT_GT(fd, 0);
  EXPECT_EQ(LIBC_NAMESPACE::write(fd, TEXT, sizeof(TEXT)),
            static_cast<ssize_t>(sizeof(TEXT)));
  EXPECT_EQ(LIBC_NAMESPACE::lseek(fd, 0, SEEK_SET), off_t(0));
  return fd;
}

} // namespace

TEST_F(LlvmLibcLockfTest, LockAndUnlockTheWholeFile) {
  auto path = libc_make_test_file_path("lockf_whole.test");
  int fd = open_with_content(path);

  // A length of zero means from where the file offset is to the end, however
  // far the file grows afterwards.
  ASSERT_THAT(LIBC_NAMESPACE::lockf(fd, F_LOCK, 0), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::lockf(fd, F_ULOCK, 0), Succeeds(0));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcLockfTest, TryLockSucceedsOnAFreeRegion) {
  auto path = libc_make_test_file_path("lockf_try.test");
  int fd = open_with_content(path);

  ASSERT_THAT(LIBC_NAMESPACE::lockf(fd, F_TLOCK, 4), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::lockf(fd, F_ULOCK, 4), Succeeds(0));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

// A test reports whether the region could be locked. A lock this process
// already holds does not stand in its own way, so the answer stays zero.
TEST_F(LlvmLibcLockfTest, TestSeesNoneOfItsOwnLocks) {
  auto path = libc_make_test_file_path("lockf_test_own.test");
  int fd = open_with_content(path);

  ASSERT_THAT(LIBC_NAMESPACE::lockf(fd, F_TEST, 0), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::lockf(fd, F_LOCK, 0), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::lockf(fd, F_TEST, 0), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::lockf(fd, F_ULOCK, 0), Succeeds(0));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcLockfTest, AnUnknownCommandIsRejected) {
  auto path = libc_make_test_file_path("lockf_bad_cmd.test");
  int fd = open_with_content(path);

  ASSERT_THAT(LIBC_NAMESPACE::lockf(fd, 42, 0), Fails(EINVAL));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcLockfTest, ADescriptorThatIsNotOneFails) {
  ASSERT_THAT(LIBC_NAMESPACE::lockf(-1, F_LOCK, 0), Fails(EBADF));
}

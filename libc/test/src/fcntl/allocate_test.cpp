//===-- Unittests for the file space calls --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/__support/libc_errno.h"
#include "src/fcntl/fallocate.h"
#include "src/fcntl/open.h"
#include "src/fcntl/posix_fadvise.h"
#include "src/fcntl/posix_fallocate.h"
#include "src/sys/stat/fstat.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include <fcntl.h>

using LlvmLibcAllocateTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

namespace {

int make_file(const char *path) {
  return LIBC_NAMESPACE::open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
}

} // anonymous namespace

// posix_fallocate reserves space and makes the file that long, and it
// reports the error rather than setting errno.
TEST_F(LlvmLibcAllocateTest, ReservesSpaceAndGrowsTheFile) {
  auto path = libc_make_test_file_path("posix_fallocate.test");
  int fd = make_file(path);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);

  ASSERT_EQ(LIBC_NAMESPACE::posix_fallocate(fd, 0, 4096), 0);
  ASSERT_ERRNO_SUCCESS();

  struct stat st;
  ASSERT_THAT(LIBC_NAMESPACE::fstat(fd, &st), Succeeds(0));
  ASSERT_EQ(st.st_size, static_cast<off_t>(4096));

  // A length of zero is not a length, and is refused.
  ASSERT_EQ(LIBC_NAMESPACE::posix_fallocate(fd, 0, 0), EINVAL);
  ASSERT_ERRNO_SUCCESS();

  ASSERT_EQ(LIBC_NAMESPACE::posix_fallocate(-1, 0, 1), EBADF);
  ASSERT_ERRNO_SUCCESS();

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

// fallocate is the Linux form, which takes a mode and reports through errno.
TEST_F(LlvmLibcAllocateTest, ReservesSpaceWithoutChangingTheLength) {
  auto path = libc_make_test_file_path("fallocate.test");
  int fd = make_file(path);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);

  int result = LIBC_NAMESPACE::fallocate(fd, FALLOC_FL_KEEP_SIZE, 0, 4096);
  if (result != 0) {
    // A filesystem need not offer it at all.
    ASSERT_ERRNO_EQ(EOPNOTSUPP);
    LIBC_NAMESPACE::libc_errno = 0;
  } else {
    struct stat st;
    ASSERT_THAT(LIBC_NAMESPACE::fstat(fd, &st), Succeeds(0));
    ASSERT_EQ(st.st_size, static_cast<off_t>(0));
  }

  ASSERT_THAT(LIBC_NAMESPACE::fallocate(-1, 0, 0, 1), Fails(EBADF));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

// posix_fadvise says how the file will be read. Nothing observable follows
// from it, so what is checked is that it accepts what it should and refuses
// what it should, reporting the error rather than setting errno.
TEST_F(LlvmLibcAllocateTest, SayingHowTheFileWillBeRead) {
  auto path = libc_make_test_file_path("posix_fadvise.test");
  int fd = make_file(path);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);

  ASSERT_EQ(LIBC_NAMESPACE::posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL), 0);
  ASSERT_ERRNO_SUCCESS();

  ASSERT_EQ(LIBC_NAMESPACE::posix_fadvise(fd, 0, 0, -1), EINVAL);
  ASSERT_ERRNO_SUCCESS();

  ASSERT_EQ(LIBC_NAMESPACE::posix_fadvise(-1, 0, 0, POSIX_FADV_NORMAL), EBADF);
  ASSERT_ERRNO_SUCCESS();

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

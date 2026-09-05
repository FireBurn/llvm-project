//===-- Unittests for posix_fallocate -------------------------------------===//
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
#include "src/fcntl/posix_fallocate.h"
#include "src/sys/stat/fstat.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcPosixFallocateTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// posix_fallocate reserves the space and makes the file that long, and it
// reports an error by returning it, leaving errno alone.
TEST_F(LlvmLibcPosixFallocateTest, ReservesSpaceAndGrowsTheFile) {
  auto path = libc_make_test_file_path("posix_fallocate_only.test");
  int fd = LIBC_NAMESPACE::open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);

  ASSERT_EQ(LIBC_NAMESPACE::posix_fallocate(fd, 0, 4096), 0);
  ASSERT_ERRNO_SUCCESS();

  struct stat st;
  ASSERT_THAT(LIBC_NAMESPACE::fstat(fd, &st), Succeeds(0));
  ASSERT_EQ(st.st_size, static_cast<off_t>(4096));

  ASSERT_EQ(LIBC_NAMESPACE::posix_fallocate(fd, 0, 0), EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::posix_fallocate(-1, 0, 1), EBADF);
  ASSERT_ERRNO_SUCCESS();

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

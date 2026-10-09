//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for futimesat
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_timeval.h"
#include "src/fcntl/open.h"
#include "src/sys/stat/stat.h"
#include "src/sys/time/futimesat.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcFutimesatTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

TEST_F(LlvmLibcFutimesatTest, ChangesTheTimesOfAPath) {
  auto path = libc_make_test_file_path("futimesat.test");
  int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  struct timeval times[2];
  times[0].tv_sec = 1000000000;
  times[0].tv_usec = 0;
  times[1].tv_sec = 1000000002;
  times[1].tv_usec = 500000;
  ASSERT_THAT(LIBC_NAMESPACE::futimesat(AT_FDCWD, path, times), Succeeds(0));

  struct stat st;
  ASSERT_THAT(LIBC_NAMESPACE::stat(path, &st), Succeeds(0));
  EXPECT_EQ(st.st_atim.tv_sec, static_cast<time_t>(1000000000));
  EXPECT_EQ(st.st_mtim.tv_sec, static_cast<time_t>(1000000002));
  EXPECT_EQ(st.st_mtim.tv_nsec, static_cast<long>(500000000));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcFutimesatTest, FailsOnAMissingPath) {
  struct timeval times[2] = {{1, 0}, {1, 0}};
  ASSERT_THAT(LIBC_NAMESPACE::futimesat(AT_FDCWD, "/nonexistent/futimesat",
                                        times),
              Fails(ENOENT));
}

TEST_F(LlvmLibcFutimesatTest, FailsOnABadMicrosecondCount) {
  auto path = libc_make_test_file_path("futimesat_bad.test");
  int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  struct timeval times[2] = {{1, 1000000}, {1, 0}};
  ASSERT_THAT(LIBC_NAMESPACE::futimesat(AT_FDCWD, path, times), Fails(EINVAL));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

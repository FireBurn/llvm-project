//===-- Unittests for futimes ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_timeval.h"
#include "src/fcntl/open.h"
#include "src/sys/stat/fstat.h"
#include "src/sys/time/futimes.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcFutimesTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

TEST_F(LlvmLibcFutimesTest, ChangesTheTimesOfAnOpenFile) {
  auto path = libc_make_test_file_path("futimes.test");
  int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);

  struct timeval times[2];
  times[0].tv_sec = 1000000000;
  times[0].tv_usec = 250000;
  times[1].tv_sec = 1000000001;
  times[1].tv_usec = 500000;
  ASSERT_THAT(LIBC_NAMESPACE::futimes(fd, times), Succeeds(0));

  struct stat st;
  ASSERT_THAT(LIBC_NAMESPACE::fstat(fd, &st), Succeeds(0));
  ASSERT_EQ(st.st_atim.tv_sec, static_cast<time_t>(1000000000));
  ASSERT_EQ(st.st_mtim.tv_sec, static_cast<time_t>(1000000001));
  ASSERT_EQ(st.st_mtim.tv_nsec, static_cast<long>(500000000));

  // Microseconds past a second are not a time.
  times[1].tv_usec = 1000000;
  ASSERT_THAT(LIBC_NAMESPACE::futimes(fd, times), Fails(EINVAL));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcFutimesTest, DescriptorThatIsNotOpen) {
  ASSERT_THAT(LIBC_NAMESPACE::futimes(-1, nullptr), Fails(EBADF));
}

//===-- Unittests for futimens --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_timespec.h"
#include "src/fcntl/open.h"
#include "src/sys/stat/fstat.h"
#include "src/sys/stat/futimens.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcFutimensTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

TEST_F(LlvmLibcFutimensTest, SetsBothTimes) {
  auto path = libc_make_test_file_path("futimens.test");
  int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);

  struct timespec times[2];
  times[0].tv_sec = 1000000000;
  times[0].tv_nsec = 0;
  times[1].tv_sec = 1000000001;
  times[1].tv_nsec = 0;
  ASSERT_THAT(LIBC_NAMESPACE::futimens(fd, times), Succeeds(0));

  struct stat st;
  ASSERT_THAT(LIBC_NAMESPACE::fstat(fd, &st), Succeeds(0));
  ASSERT_EQ(st.st_atim.tv_sec, static_cast<time_t>(1000000000));
  ASSERT_EQ(st.st_mtim.tv_sec, static_cast<time_t>(1000000001));

  // A null pointer means the time now, which is later than what was set.
  ASSERT_THAT(LIBC_NAMESPACE::futimens(fd, nullptr), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::fstat(fd, &st), Succeeds(0));
  ASSERT_GT(st.st_mtim.tv_sec, static_cast<time_t>(1000000001));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcFutimensTest, DescriptorThatIsNotOne) {
  ASSERT_THAT(LIBC_NAMESPACE::futimens(-1, nullptr), Fails(EBADF));
}

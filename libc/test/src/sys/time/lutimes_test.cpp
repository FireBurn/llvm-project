//===-- Unittests for lutimes ---------------------------------------------===//
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
#include "src/sys/stat/lstat.h"
#include "src/sys/stat/stat.h"
#include "src/sys/time/lutimes.h"
#include "src/unistd/close.h"
#include "src/unistd/symlink.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcLutimesTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// lutimes sets the times on the link itself, leaving what it points at
// alone, which is the whole of what separates it from utimes.
TEST_F(LlvmLibcLutimesTest, ChangesTheLinkAndNotTheTarget) {
  auto target = libc_make_test_file_path("lutimes_target.test");
  auto link = libc_make_test_file_path("lutimes_link.test");

  int fd = LIBC_NAMESPACE::open(target, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::symlink(target, link), Succeeds(0));

  struct stat before;
  ASSERT_THAT(LIBC_NAMESPACE::stat(target, &before), Succeeds(0));

  struct timeval times[2];
  times[0].tv_sec = 1000000000;
  times[0].tv_usec = 0;
  times[1].tv_sec = 1000000001;
  times[1].tv_usec = 0;
  ASSERT_THAT(LIBC_NAMESPACE::lutimes(link, times), Succeeds(0));

  struct stat link_stat;
  ASSERT_THAT(LIBC_NAMESPACE::lstat(link, &link_stat), Succeeds(0));
  ASSERT_EQ(link_stat.st_mtim.tv_sec, static_cast<time_t>(1000000001));

  struct stat after;
  ASSERT_THAT(LIBC_NAMESPACE::stat(target, &after), Succeeds(0));
  ASSERT_EQ(after.st_mtim.tv_sec, before.st_mtim.tv_sec);

  ASSERT_THAT(LIBC_NAMESPACE::unlink(link), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(target), Succeeds(0));
}

TEST_F(LlvmLibcLutimesTest, PathThatIsNotThere) {
  ASSERT_THAT(LIBC_NAMESPACE::lutimes("/nonexistent/llvm_libc", nullptr),
              Fails(ENOENT));
}

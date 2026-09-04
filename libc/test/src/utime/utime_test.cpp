//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for utime.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_stat.h"
#include "hdr/types/struct_utimbuf.h"
#include "src/fcntl/open.h"
#include "src/sys/stat/stat.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "src/utime/utime.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcUtimeTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcUtimeTest, SetsBothTimes) {
  constexpr const char *FILENAME = "utime.test";
  int fd =
      LIBC_NAMESPACE::open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  struct utimbuf times;
  times.actime = 123456;
  times.modtime = 654321;
  ASSERT_THAT(LIBC_NAMESPACE::utime(FILENAME, &times), Succeeds(0));

  struct stat statbuf;
  ASSERT_THAT(LIBC_NAMESPACE::stat(FILENAME, &statbuf), Succeeds(0));
  EXPECT_EQ(statbuf.st_atim.tv_sec,
            static_cast<decltype(statbuf.st_atim.tv_sec)>(123456));
  EXPECT_EQ(statbuf.st_mtim.tv_sec,
            static_cast<decltype(statbuf.st_mtim.tv_sec)>(654321));

  ASSERT_THAT(LIBC_NAMESPACE::unlink(FILENAME), Succeeds(0));
}

TEST_F(LlvmLibcUtimeTest, NullTimesUsesNow) {
  constexpr const char *FILENAME = "utime_now.test";
  int fd =
      LIBC_NAMESPACE::open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  struct utimbuf old;
  old.actime = 1;
  old.modtime = 1;
  ASSERT_THAT(LIBC_NAMESPACE::utime(FILENAME, &old), Succeeds(0));

  // A null pointer asks for the current time, which is well past 1970.
  ASSERT_THAT(LIBC_NAMESPACE::utime(FILENAME, nullptr), Succeeds(0));

  struct stat statbuf;
  ASSERT_THAT(LIBC_NAMESPACE::stat(FILENAME, &statbuf), Succeeds(0));
  EXPECT_GT(statbuf.st_mtim.tv_sec,
            static_cast<decltype(statbuf.st_mtim.tv_sec)>(1));

  ASSERT_THAT(LIBC_NAMESPACE::unlink(FILENAME), Succeeds(0));
}

TEST_F(LlvmLibcUtimeTest, MissingFile) {
  struct utimbuf times;
  times.actime = 0;
  times.modtime = 0;
  EXPECT_THAT(LIBC_NAMESPACE::utime("utime_no_such_file.test", &times),
              Fails(ENOENT, -1));
}

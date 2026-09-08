//===-- Unittests for umask -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/fcntl/open.h"
#include "src/sys/stat/stat.h"
#include "src/sys/stat/umask.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcUmaskTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// umask cannot fail, and reports the mask that was in force rather than the
// one just set.
TEST_F(LlvmLibcUmaskTest, ReportsTheMaskItReplaced) {
  mode_t before = LIBC_NAMESPACE::umask(0);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_EQ(LIBC_NAMESPACE::umask(before), static_cast<mode_t>(0));
  ASSERT_EQ(LIBC_NAMESPACE::umask(before), before);
}

// The bits set in the mask are the bits a newly made file does not get.
TEST_F(LlvmLibcUmaskTest, TakesPermissionsOffANewFile) {
  auto path = libc_make_test_file_path("umask.test");
  mode_t before = LIBC_NAMESPACE::umask(S_IRWXG | S_IRWXO);

  int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT | O_TRUNC, 0777);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  struct stat st;
  ASSERT_THAT(LIBC_NAMESPACE::stat(path, &st), Succeeds(0));
  ASSERT_EQ(st.st_mode & static_cast<mode_t>(S_IRWXG | S_IRWXO),
            static_cast<mode_t>(0));
  ASSERT_EQ(st.st_mode & static_cast<mode_t>(S_IRWXU),
            static_cast<mode_t>(S_IRWXU));

  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
  LIBC_NAMESPACE::umask(before);
}

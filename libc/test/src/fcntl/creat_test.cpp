//===-- Unittest for creat ------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/fcntl/creat.h"
#include "src/fcntl/open.h"
#include "src/unistd/close.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcCreatTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcCreatTest, CreatAndOpen) {
  using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;
  constexpr const char *TEST_FILE = "testdata/creat.test";
  int fd = LIBC_NAMESPACE::creat(TEST_FILE, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  fd = LIBC_NAMESPACE::open(TEST_FILE, O_RDONLY);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  // TODO: 'remove' the test file at the end.
}

// O_LARGEFILE must not collide with another flag: it was O_DIRECTORY on aarch64, and the kernel refuses O_CREAT with that.
TEST_F(LlvmLibcCreatTest, OpenWithLargeFileFlag) {
  using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;
  static_assert((O_LARGEFILE & (O_DIRECTORY | O_NOFOLLOW | O_CREAT)) == 0,
                "O_LARGEFILE overlaps another open flag");
  constexpr const char *TEST_FILE = "testdata/open_largefile.test";
  int fd = LIBC_NAMESPACE::open(
      TEST_FILE, O_RDWR | O_CREAT | O_LARGEFILE | O_NOFOLLOW | O_CLOEXEC,
      S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
}

//===-- Unittests for close_range -----------------------------------------===//
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
#include "src/fcntl/open.h"
#include "src/unistd/close.h"
#include "src/unistd/close_range.h"
#include "src/unistd/dup.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcCloseRangeTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

TEST_F(LlvmLibcCloseRangeTest, ClosesEveryDescriptorInTheRange) {
  auto path = libc_make_test_file_path("close_range.test");
  int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);

  int second = LIBC_NAMESPACE::dup(fd);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(second, fd);

  int low = fd < second ? fd : second;
  int high = fd < second ? second : fd;
  ASSERT_THAT(LIBC_NAMESPACE::close_range(static_cast<unsigned>(low),
                                          static_cast<unsigned>(high), 0),
              Succeeds(0));

  // Both are gone, so closing either again is refused.
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Fails(EBADF));
  ASSERT_THAT(LIBC_NAMESPACE::close(second), Fails(EBADF));

  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

// A range holding nothing open is not an error.
TEST_F(LlvmLibcCloseRangeTest, ARangeWithNothingInIt) {
  ASSERT_THAT(LIBC_NAMESPACE::close_range(90000, 90010, 0), Succeeds(0));
}

// The range has to run upwards.
TEST_F(LlvmLibcCloseRangeTest, ARangeThatRunsBackwards) {
  ASSERT_THAT(LIBC_NAMESPACE::close_range(20, 10, 0), Fails(EINVAL));
}

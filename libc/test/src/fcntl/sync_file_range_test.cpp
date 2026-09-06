//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for sync_file_range
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/fcntl/open.h"
#include "src/fcntl/sync_file_range.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcSyncFileRangeTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

TEST_F(LlvmLibcSyncFileRangeTest, WritesBackPartOfAFile) {
  constexpr char TEXT[] = "sync_file_range";
  auto path = libc_make_test_file_path("sync_file_range.test");
  int fd = LIBC_NAMESPACE::open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::write(fd, TEXT, sizeof(TEXT)),
              Succeeds<ssize_t>(sizeof(TEXT)));

  ASSERT_THAT(LIBC_NAMESPACE::sync_file_range(fd, 0, 0, 0), Succeeds(0));

  ASSERT_THAT(LIBC_NAMESPACE::sync_file_range(-1, 0, 0, 0), Fails(EBADF));
  ASSERT_THAT(LIBC_NAMESPACE::sync_file_range(fd, -1, 0, 0), Fails(EINVAL));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

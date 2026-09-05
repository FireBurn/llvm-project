//===-- Unittests for sync and syncfs -------------------------------------===//
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
#include "src/unistd/close.h"
#include "src/unistd/sync.h"
#include "src/unistd/syncfs.h"
#include "src/unistd/unlink.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcSyncTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// sync returns nothing and cannot fail; what is checked is that it leaves
// errno alone.
TEST_F(LlvmLibcSyncTest, SyncSaysNothing) {
  LIBC_NAMESPACE::sync();
  ASSERT_ERRNO_SUCCESS();
}

TEST_F(LlvmLibcSyncTest, SyncfsOnAFileWeOpened) {
  constexpr char TEXT[] = "syncfs";
  auto path = libc_make_test_file_path("syncfs.test");
  int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);

  ASSERT_THAT(LIBC_NAMESPACE::write(fd, TEXT, sizeof(TEXT)),
              Succeeds<ssize_t>(sizeof(TEXT)));
  ASSERT_THAT(LIBC_NAMESPACE::syncfs(fd), Succeeds(0));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcSyncTest, SyncfsOnADescriptorThatIsNotOne) {
  ASSERT_THAT(LIBC_NAMESPACE::syncfs(-1), Fails(EBADF));
}

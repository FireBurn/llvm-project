//===-- Unittests for fanotify_init and fanotify_mark ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "src/__support/libc_errno.h"
#include "src/sys/fanotify/fanotify_init.h"
#include "src/sys/fanotify/fanotify_mark.h"
#include "src/unistd/close.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include <sys/fanotify.h>

using LlvmLibcFanotifyTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// Watching a filesystem needs CAP_SYS_ADMIN, so a test that is not root is
// refused. Where it does have the privilege the group is made and marked,
// which is undone again by closing it.
TEST_F(LlvmLibcFanotifyTest, MakingAGroup) {
  int fd = LIBC_NAMESPACE::fanotify_init(FAN_CLASS_NOTIF, O_RDONLY);
  if (fd < 0) {
    ASSERT_ERRNO_EQ(EPERM);
    LIBC_NAMESPACE::libc_errno = 0;
    return;
  }

  ASSERT_THAT(
      LIBC_NAMESPACE::fanotify_mark(fd, FAN_MARK_ADD, FAN_OPEN, AT_FDCWD, "."),
      Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::fanotify_mark(fd, FAN_MARK_REMOVE, FAN_OPEN,
                                            AT_FDCWD, "."),
              Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
}

// Flags that do not go together are refused before any privilege is looked
// at, so this says the arguments reach the kernel the way it reads them.
TEST_F(LlvmLibcFanotifyTest, FlagsThatDoNotGoTogether) {
  int result = LIBC_NAMESPACE::fanotify_init(0xffffffffU, O_RDONLY);
  ASSERT_EQ(result, -1);
  int err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == EINVAL || err == EPERM);
  LIBC_NAMESPACE::libc_errno = 0;
}

TEST_F(LlvmLibcFanotifyTest, DescriptorThatIsNotOne) {
  ASSERT_THAT(
      LIBC_NAMESPACE::fanotify_mark(-1, FAN_MARK_ADD, FAN_OPEN, AT_FDCWD, "."),
      Fails(EBADF));
}

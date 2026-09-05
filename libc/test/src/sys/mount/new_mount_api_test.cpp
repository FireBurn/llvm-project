//===-- Unittests for the newer mount calls -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_mount_macros.h"
#include "hdr/types/struct_mount_attr.h"
#include "src/__support/libc_errno.h"
#include "src/sys/mount/fsconfig.h"
#include "src/sys/mount/fsmount.h"
#include "src/sys/mount/fsopen.h"
#include "src/sys/mount/fspick.h"
#include "src/sys/mount/mount_setattr.h"
#include "src/sys/mount/move_mount.h"
#include "src/sys/mount/open_tree.h"
#include "src/sys/mount/pivot_root.h"
#include "src/unistd/close.h"
#include "src/unistd/geteuid.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcNewMountApiTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// Mounting a filesystem needs CAP_SYS_ADMIN, which a test does not have, and
// taking it where the test does run as root would change the machine. What
// is checked is that each call reaches the kernel with its arguments in the
// right places, which the answers say.

TEST_F(LlvmLibcNewMountApiTest, OpeningAFilesystemContext) {
  int fd = LIBC_NAMESPACE::fsopen("tmpfs", 0);
  if (fd < 0) {
    ASSERT_ERRNO_EQ(EPERM);
    LIBC_NAMESPACE::libc_errno = 0;
    return;
  }

  // The context is open but nothing has been created from it yet, so asking
  // for a mount out of it is refused rather than granted.
  int mount_fd = LIBC_NAMESPACE::fsmount(fd, 0, 0);
  if (mount_fd >= 0)
    LIBC_NAMESPACE::close(mount_fd);
  else
    LIBC_NAMESPACE::libc_errno = 0;

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
}

TEST_F(LlvmLibcNewMountApiTest, AFilesystemThatIsNotOne) {
  int result = LIBC_NAMESPACE::fsopen("llvm-libc-not-a-filesystem", 0);
  ASSERT_EQ(result, -1);
  int err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == ENODEV || err == EPERM);
  LIBC_NAMESPACE::libc_errno = 0;
}

TEST_F(LlvmLibcNewMountApiTest, DescriptorThatIsNotOne) {
  // fsmount and move_mount look at the privilege before the descriptor, so
  // a caller without it is told EPERM rather than which descriptor it gave.
  ASSERT_EQ(LIBC_NAMESPACE::fsmount(-1, 0, 0), -1);
  int err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == EBADF || err == EPERM);
  LIBC_NAMESPACE::libc_errno = 0;

  ASSERT_EQ(LIBC_NAMESPACE::move_mount(-1, "", AT_FDCWD, "/",
                                       MOVE_MOUNT_F_EMPTY_PATH),
            -1);
  err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == EBADF || err == EPERM);
  LIBC_NAMESPACE::libc_errno = 0;

  // The commands are an enumeration in the kernel's own header, which is
  // not included here; zero is the first of them, FSCONFIG_SET_FLAG. The
  // kernel checks what it was asked to do before which descriptor it was
  // given, so a command it cannot carry out is reported first.
  ASSERT_EQ(LIBC_NAMESPACE::fsconfig(-1, 0, "ro", nullptr, 0), -1);
  err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == EBADF || err == EINVAL);
  LIBC_NAMESPACE::libc_errno = 0;
}

TEST_F(LlvmLibcNewMountApiTest, PathThatIsNotThere) {
  int result = LIBC_NAMESPACE::open_tree(AT_FDCWD, "llvm-libc-not-here", 0);
  ASSERT_EQ(result, -1);
  int err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == ENOENT || err == EPERM);
  LIBC_NAMESPACE::libc_errno = 0;

  result = LIBC_NAMESPACE::fspick(AT_FDCWD, "llvm-libc-not-here", 0);
  ASSERT_EQ(result, -1);
  err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == ENOENT || err == EPERM || err == EINVAL);
  LIBC_NAMESPACE::libc_errno = 0;

  struct mount_attr attr = {};
  result = LIBC_NAMESPACE::mount_setattr(AT_FDCWD, "llvm-libc-not-here", 0,
                                         &attr, sizeof(attr));
  ASSERT_EQ(result, -1);
  err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == ENOENT || err == EPERM);
  LIBC_NAMESPACE::libc_errno = 0;
}

// Changing the root of the mount namespace is refused without the
// privilege, and refused again for a path that is not a mount point.
TEST_F(LlvmLibcNewMountApiTest, ChangingTheRoot) {
  int result = LIBC_NAMESPACE::pivot_root("llvm-libc-not-here",
                                          "llvm-libc-not-here-either");
  ASSERT_EQ(result, -1);
  int err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == EPERM || err == ENOENT || err == EINVAL);
  LIBC_NAMESPACE::libc_errno = 0;
}

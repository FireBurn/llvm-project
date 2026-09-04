//===-- Unittests for mount, umount and umount2 ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/sys_mount_macros.h"
#include "src/sys/mount/mount.h"
#include "src/sys/mount/umount.h"
#include "src/sys/mount/umount2.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcMountTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// Mounting needs CAP_SYS_ADMIN, which the tests do not have, so what is
// checked is that the call reaches the kernel and comes back with the
// kernel's own refusal rather than something of the libc's making.

TEST_F(LlvmLibcMountTest, NeedsPrivilege) {
  EXPECT_THAT(LIBC_NAMESPACE::mount("none", "/mnt", "tmpfs", 0, nullptr),
              Fails(any_of(EPERM, EACCES, ENOENT), -1));
}

TEST_F(LlvmLibcMountTest, UnmountingSomethingWhichIsNotAMountPoint) {
  EXPECT_THAT(LIBC_NAMESPACE::umount("/"),
              Fails(any_of(EPERM, EINVAL, EBUSY), -1));
  EXPECT_THAT(LIBC_NAMESPACE::umount2("/", MNT_DETACH),
              Fails(any_of(EPERM, EINVAL, EBUSY), -1));
}

TEST_F(LlvmLibcMountTest, MissingPath) {
  EXPECT_THAT(LIBC_NAMESPACE::umount("/no/such/mount/point"),
              Fails(any_of(EPERM, ENOENT), -1));
  EXPECT_THAT(LIBC_NAMESPACE::umount2("/no/such/mount/point", MNT_FORCE),
              Fails(any_of(EPERM, ENOENT), -1));
}

//===-- Unittests for the linux/sys/mount macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the linux sys mount macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/sys-mount-macros.h"

TEST(LlvmLibcLinuxSysMountMacrosTest,
     FillInTheMissingFcntlStatxAndMountMacros) {
  EXPECT_EQ(static_cast<long long>(MOUNT_ATTR_RDONLY), 1LL);
  EXPECT_EQ(static_cast<long long>(MOUNT_ATTR_NOSUID), 2LL);
  EXPECT_EQ(static_cast<long long>(MOUNT_ATTR_NODEV), 4LL);
  EXPECT_EQ(static_cast<long long>(MOUNT_ATTR_NOEXEC), 8LL);
  EXPECT_EQ(static_cast<long long>(MOUNT_ATTR__ATIME), 112LL);
  EXPECT_EQ(static_cast<long long>(MOUNT_ATTR_RELATIME), 0LL);
  EXPECT_EQ(static_cast<long long>(MOUNT_ATTR_NOATIME), 16LL);
  EXPECT_EQ(static_cast<long long>(MOUNT_ATTR_STRICTATIME), 32LL);
  EXPECT_EQ(static_cast<long long>(MOUNT_ATTR_NODIRATIME), 128LL);
  EXPECT_EQ(static_cast<long long>(MOUNT_ATTR_IDMAP), 0x100000LL);
  EXPECT_EQ(static_cast<long long>(MOUNT_ATTR_NOSYMFOLLOW), 0x200000LL);
  EXPECT_EQ(static_cast<long long>(MOUNT_ATTR_SIZE_VER0), 32LL);
}

//===-- Unittests for the linux/sys/stat macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the linux sys stat macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/sys-stat-macros.h"

TEST(LlvmLibcLinuxSysStatMacrosTest, FillInTheMissingFcntlStatxAndMountMacros) {
  EXPECT_EQ(static_cast<long long>(STATX_TYPE), 1LL);
  EXPECT_EQ(static_cast<long long>(STATX_MODE), 2LL);
  EXPECT_EQ(static_cast<long long>(STATX_NLINK), 4LL);
  EXPECT_EQ(static_cast<long long>(STATX_UID), 8LL);
  EXPECT_EQ(static_cast<long long>(STATX_GID), 16LL);
  EXPECT_EQ(static_cast<long long>(STATX_ATIME), 32LL);
  EXPECT_EQ(static_cast<long long>(STATX_MTIME), 64LL);
  EXPECT_EQ(static_cast<long long>(STATX_CTIME), 128LL);
  EXPECT_EQ(static_cast<long long>(STATX_INO), 256LL);
  EXPECT_EQ(static_cast<long long>(STATX_SIZE), 512LL);
  EXPECT_EQ(static_cast<long long>(STATX_BLOCKS), 1024LL);
  EXPECT_EQ(static_cast<long long>(STATX_BASIC_STATS), 2047LL);
  EXPECT_EQ(static_cast<long long>(STATX_BTIME), 2048LL);
  EXPECT_EQ(static_cast<long long>(STATX_MNT_ID), 0x1000LL);
  EXPECT_EQ(static_cast<long long>(STATX_DIOALIGN), 0x2000LL);
  EXPECT_EQ(static_cast<long long>(STATX_MNT_ID_UNIQUE), 0x4000LL);
  EXPECT_EQ(static_cast<long long>(STATX_SUBVOL), 0x8000LL);
  EXPECT_EQ(static_cast<long long>(STATX_WRITE_ATOMIC), 0x10000LL);
  EXPECT_EQ(static_cast<long long>(STATX_DIO_READ_ALIGN), 0x20000LL);
  EXPECT_EQ(static_cast<long long>(STATX_ALL), 4095LL);
  EXPECT_EQ(static_cast<long long>(STATX_ATTR_COMPRESSED), 4LL);
  EXPECT_EQ(static_cast<long long>(STATX_ATTR_IMMUTABLE), 16LL);
  EXPECT_EQ(static_cast<long long>(STATX_ATTR_APPEND), 32LL);
  EXPECT_EQ(static_cast<long long>(STATX_ATTR_NODUMP), 64LL);
  EXPECT_EQ(static_cast<long long>(STATX_ATTR_ENCRYPTED), 2048LL);
  EXPECT_EQ(static_cast<long long>(STATX_ATTR_AUTOMOUNT), 0x1000LL);
  EXPECT_EQ(static_cast<long long>(STATX_ATTR_MOUNT_ROOT), 0x2000LL);
  EXPECT_EQ(static_cast<long long>(STATX_ATTR_VERITY), 0x100000LL);
  EXPECT_EQ(static_cast<long long>(STATX_ATTR_DAX), 0x200000LL);
  EXPECT_EQ(static_cast<long long>(STATX_ATTR_WRITE_ATOMIC), 0x400000LL);
}

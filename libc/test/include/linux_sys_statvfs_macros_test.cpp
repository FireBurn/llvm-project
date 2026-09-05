//===-- Unittests for the linux/sys/statvfs macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the linux sys statvfs macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/sys-statvfs-macros.h"

TEST(LlvmLibcLinuxSysStatvfsMacrosTest, AddTheSt_FlagsToSysStatvfsH) {
  EXPECT_EQ(static_cast<long long>(ST_RDONLY), 1LL);
  EXPECT_EQ(static_cast<long long>(ST_NOSUID), 2LL);
  EXPECT_EQ(static_cast<long long>(ST_NODEV), 4LL);
  EXPECT_EQ(static_cast<long long>(ST_NOEXEC), 8LL);
  EXPECT_EQ(static_cast<long long>(ST_SYNCHRONOUS), 16LL);
  EXPECT_EQ(static_cast<long long>(ST_MANDLOCK), 64LL);
  EXPECT_EQ(static_cast<long long>(ST_WRITE), 128LL);
  EXPECT_EQ(static_cast<long long>(ST_APPEND), 256LL);
  EXPECT_EQ(static_cast<long long>(ST_IMMUTABLE), 512LL);
  EXPECT_EQ(static_cast<long long>(ST_NOATIME), 1024LL);
  EXPECT_EQ(static_cast<long long>(ST_NODIRATIME), 2048LL);
  EXPECT_EQ(static_cast<long long>(ST_RELATIME), 0x1000LL);
  EXPECT_EQ(static_cast<long long>(ST_NOSYMFOLLOW), 0x2000LL);
}

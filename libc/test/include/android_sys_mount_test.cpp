//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that sys/mount.h gives Android programs the flags of linux/fs.h.
///
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/sys-mount-macros.h"

TEST(LlvmLibcAndroidSysMountTest, KernelFsFlagsComeWithMountHeader) {
  EXPECT_EQ(static_cast<long long>(FS_CASEFOLD_FL), 0x40000000LL);
  EXPECT_EQ(static_cast<long long>(FS_IMMUTABLE_FL), 0x00000010LL);
}

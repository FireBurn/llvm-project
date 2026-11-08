//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that sys/socket.h gives Android programs the ioctl numbers of
/// linux/sockios.h.
///
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/sys-socket-macros.h"

TEST(LlvmLibcAndroidSysSocketTest, KernelIoctlNumbersComeWithSocketHeader) {
  EXPECT_EQ(static_cast<int>(SIOCSIFADDR), 0x8916);
  EXPECT_EQ(static_cast<int>(SIOCGIFFLAGS), 0x8913);
}

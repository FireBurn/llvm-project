//===-- Unittests for the includes Android code relies on -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "test/UnitTest/Test.h"

// Each header is the only one included for what it must declare, so that
// nothing else can supply it.
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/xattr.h>
#include <termios.h>

TEST(LlvmLibcAndroidPosixIncludesTest, StdlibDeclaresMallopt) {
  EXPECT_EQ(M_PURGE_ALL, -104);
}

TEST(LlvmLibcAndroidPosixIncludesTest, WaitDeclaresResourceConstants) {
  EXPECT_EQ(PRIO_PROCESS, 0);
}

TEST(LlvmLibcAndroidPosixIncludesTest, TermiosDeclaresTerminalRequests) {
  EXPECT_EQ(TIOCSCTTY, 0x540e);
}

TEST(LlvmLibcAndroidPosixIncludesTest, XattrDeclaresKernelNames) {
  EXPECT_STREQ(XATTR_NAME_SELINUX, "security.selinux");
}

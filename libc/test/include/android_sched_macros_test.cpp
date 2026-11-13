//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
///
/// \file
/// Tests that Android programs get SCHED_NORMAL for the default policy and
/// can include <linux/sched.h> beside the macros.
///
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "include/llvm-libc-macros/linux/sched-macros.h"
#include "test/UnitTest/Test.h"

// Android programs include the kernel's header too; where the two spell a
// macro differently the compiler reports a redefinition.
#include <linux/sched.h>

TEST(LlvmLibcAndroidSchedMacrosTest, SchedNormalIsTheDefaultPolicy) {
  EXPECT_EQ(static_cast<int>(SCHED_NORMAL), 0);
  EXPECT_EQ(static_cast<int>(SCHED_NORMAL), static_cast<int>(SCHED_OTHER));
}

//===-- Unittests for the constants in sys/klog.h -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests that sys/klog.h defines the actions klogctl takes, with the
/// values of the kernel's syslog interface.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include <sys/klog.h>

TEST(LlvmLibcSysKlogTest, ActionsHaveTheKernelValues) {
  EXPECT_EQ(KLOG_CLOSE, 0);
  EXPECT_EQ(KLOG_OPEN, 1);
  EXPECT_EQ(KLOG_READ, 2);
  EXPECT_EQ(KLOG_READ_ALL, 3);
  EXPECT_EQ(KLOG_READ_CLEAR, 4);
  EXPECT_EQ(KLOG_CLEAR, 5);
  EXPECT_EQ(KLOG_CONSOLE_OFF, 6);
  EXPECT_EQ(KLOG_CONSOLE_ON, 7);
  EXPECT_EQ(KLOG_CONSOLE_LEVEL, 8);
  EXPECT_EQ(KLOG_SIZE_UNREAD, 9);
  EXPECT_EQ(KLOG_SIZE_BUFFER, 10);
}

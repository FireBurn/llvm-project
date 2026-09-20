//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the sys/wait.h status macros.
///
//===----------------------------------------------------------------------===//

#include "include/llvm-libc-macros/sys-wait-macros.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysWaitTest, Exited) {
  int status = W_EXITCODE(3, 0);
  EXPECT_TRUE(WIFEXITED(status));
  EXPECT_EQ(WEXITSTATUS(status), 3);
  EXPECT_FALSE(WIFSIGNALED(status));
  EXPECT_FALSE(WIFSTOPPED(status));
  EXPECT_FALSE(WIFCONTINUED(status));
}

TEST(LlvmLibcSysWaitTest, Signaled) {
  int status = W_EXITCODE(0, 9);
  EXPECT_TRUE(WIFSIGNALED(status));
  EXPECT_EQ(WTERMSIG(status), 9);
  EXPECT_EQ(WCOREDUMP(status), 0);
  EXPECT_FALSE(WIFEXITED(status));
  EXPECT_FALSE(WIFSTOPPED(status));
  EXPECT_FALSE(WIFCONTINUED(status));

  status |= WCOREFLAG;
  EXPECT_TRUE(WIFSIGNALED(status));
  EXPECT_EQ(WTERMSIG(status), 9);
  EXPECT_NE(WCOREDUMP(status), 0);
}

TEST(LlvmLibcSysWaitTest, Stopped) {
  int status = W_STOPCODE(19);
  EXPECT_TRUE(WIFSTOPPED(status));
  EXPECT_EQ(WSTOPSIG(status), 19);
  EXPECT_FALSE(WIFEXITED(status));
  EXPECT_FALSE(WIFSIGNALED(status));
  EXPECT_FALSE(WIFCONTINUED(status));
}

TEST(LlvmLibcSysWaitTest, Continued) {
  int status = 0xffff;
  EXPECT_TRUE(WIFCONTINUED(status));
  EXPECT_FALSE(WIFEXITED(status));
  EXPECT_FALSE(WIFSIGNALED(status));
  EXPECT_FALSE(WIFSTOPPED(status));
}

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the tests of sys_siglist and sys_signame.
///
//===----------------------------------------------------------------------===//

#include "hdr/signal_macros.h"
#include "src/signal/sys_siglist.h"
#include "src/signal/sys_signame.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysSiglistTest, DescribesTheSignals) {
  ASSERT_STREQ(LIBC_NAMESPACE::sys_siglist[SIGHUP], "Hangup");
  ASSERT_STREQ(LIBC_NAMESPACE::sys_siglist[SIGSEGV], "Segmentation fault");
  ASSERT_STREQ(LIBC_NAMESPACE::sys_siglist[SIGWINCH], "Window changed");
}

TEST(LlvmLibcSysSiglistTest, LeavesOutSignalZeroAndTheRealTimeSignals) {
  ASSERT_TRUE(LIBC_NAMESPACE::sys_siglist[0] == nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::sys_siglist[SIGRTMIN] == nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::sys_siglist[SIGRTMAX] == nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::sys_signame[0] == nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::sys_signame[SIGRTMIN] == nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::sys_signame[SIGRTMAX] == nullptr);
}

TEST(LlvmLibcSysSigNameTest, NamesTheSignalsWithoutThePrefix) {
  ASSERT_STREQ(LIBC_NAMESPACE::sys_signame[SIGHUP], "HUP");
  ASSERT_STREQ(LIBC_NAMESPACE::sys_signame[SIGKILL], "KILL");
  ASSERT_STREQ(LIBC_NAMESPACE::sys_signame[SIGSYS], "SYS");
}

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for sys/wait.h bringing in signal.h
///
//===----------------------------------------------------------------------===//

#include "include/sys/wait.h"
#include "test/UnitTest/Test.h"

#ifndef _LLVM_LIBC_SIGNAL_H
#error "<sys/wait.h> does not bring in <signal.h>"
#endif

TEST(LlvmLibcSysWaitSignalTest, CarriesTheHeaders) {
  EXPECT_NE(SIGCHLD, 0);
  EXPECT_TRUE(WIFEXITED(0));
}

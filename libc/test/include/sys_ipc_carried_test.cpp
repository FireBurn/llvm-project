//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the IPC headers carrying the macros of sys/ipc.h
///
//===----------------------------------------------------------------------===//

#include "include/sys/msg.h"
#include "include/sys/sem.h"
#include "include/sys/shm.h"
#include "test/UnitTest/Test.h"

#ifndef _LLVM_LIBC_SYS_IPC_H
#error "the IPC headers do not bring in <sys/ipc.h>"
#endif

TEST(LlvmLibcSysIpcCarriedTest, TheCommonFlagsAreVisible) {
  EXPECT_NE(IPC_CREAT, 0);
  EXPECT_NE(IPC_EXCL, 0);
  EXPECT_NE(IPC_RMID, IPC_STAT);
}

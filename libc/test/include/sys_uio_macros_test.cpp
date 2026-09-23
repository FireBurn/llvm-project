//===-- Unittests for the sys/uio macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the sys uio macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/sys-uio-macros.h"

TEST(LlvmLibcSysUioMacrosTest, DefineUio_Maxiov) {
  EXPECT_EQ(static_cast<long long>(UIO_MAXIOV), 1024LL);
}

TEST(LlvmLibcSysUioMacrosTest, DefineRwfFlags) {
  EXPECT_EQ(RWF_HIPRI, 0x1);
  EXPECT_EQ(RWF_DSYNC, 0x2);
  EXPECT_EQ(RWF_SYNC, 0x4);
  EXPECT_EQ(RWF_NOWAIT, 0x8);
  EXPECT_EQ(RWF_APPEND, 0x10);
}

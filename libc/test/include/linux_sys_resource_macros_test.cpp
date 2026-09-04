//===-- Unittests for the linux/sys/resource macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the linux sys resource macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/sys-resource-macros.h"

TEST(LlvmLibcLinuxSysResourceMacrosTest, AddRlim_Saved_CurAndRlim_Saved_Max) {
  EXPECT_EQ(static_cast<long long>(RLIM_SAVED_CUR), -1LL);
  EXPECT_EQ(static_cast<long long>(RLIM_SAVED_MAX), -1LL);
}

//===-- Unittests for the limits macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the sysexits macros.
///
//===----------------------------------------------------------------------===//


#include "include/llvm-libc-macros/sysexits-macros.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysexitsMacrosTest, ReservedRange) {
  EXPECT_EQ(EX__BASE, 64);
  EXPECT_EQ(EX__MAX, 78);
  EXPECT_EQ(EX_USAGE, EX__BASE);
  EXPECT_EQ(EX_CONFIG, EX__MAX);
}

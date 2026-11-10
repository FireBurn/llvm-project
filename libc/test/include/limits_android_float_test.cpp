//===-- Unittests for the limits macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that limits.h gives Android programs the float.h macros, as bionic does.
///
//===----------------------------------------------------------------------===//


#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "include/limits.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcLimitsAndroidFloatTest, FloatMacrosComeWithLimitsH) {
  EXPECT_TRUE(FLT_MAX > 1.0e38f);
  EXPECT_TRUE(FLT_EPSILON < 1.0e-6f);
}

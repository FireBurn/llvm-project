//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the C23 macros of float.h.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/float-macros.h"

TEST(LlvmLibcFloatMacrosTest, NormMax) {
  EXPECT_TRUE(FLT_NORM_MAX == FLT_MAX);
  EXPECT_TRUE(DBL_NORM_MAX == DBL_MAX);
  EXPECT_TRUE(LDBL_NORM_MAX == LDBL_MAX);
}

TEST(LlvmLibcFloatMacrosTest, IsIec60559) {
  EXPECT_TRUE(FLT_IS_IEC_60559 >= 0 && FLT_IS_IEC_60559 <= 2);
  EXPECT_TRUE(DBL_IS_IEC_60559 >= 0 && DBL_IS_IEC_60559 <= 2);
  EXPECT_TRUE(LDBL_IS_IEC_60559 >= 0 && LDBL_IS_IEC_60559 <= 2);
}

TEST(LlvmLibcFloatMacrosTest, SignalingNaN) {
  float f = FLT_SNAN;
  double d = DBL_SNAN;
  long double l = LDBL_SNAN;
  EXPECT_TRUE(f != f);
  EXPECT_TRUE(d != d);
  EXPECT_TRUE(l != l);
}

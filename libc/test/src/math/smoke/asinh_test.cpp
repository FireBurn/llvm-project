//===-- Unittests for asinh ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/math_macros.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/math/asinh.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcAsinhTest = LIBC_NAMESPACE::testing::FPTest<double>;

TEST_F(LlvmLibcAsinhTest, SpecialNumbers) {
  EXPECT_FP_EQ_WITH_EXCEPTION(aNaN, LIBC_NAMESPACE::asinh(sNaN), FE_INVALID);
  EXPECT_FP_EQ(aNaN, LIBC_NAMESPACE::asinh(aNaN));
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::asinh(0.0));
  EXPECT_FP_EQ(-0.0, LIBC_NAMESPACE::asinh(-0.0));
  // asinh is defined everywhere and runs out to infinity with its argument.
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::asinh(inf));
  EXPECT_FP_EQ(neg_inf, LIBC_NAMESPACE::asinh(neg_inf));
}

TEST_F(LlvmLibcAsinhTest, SmallValues) {
  EXPECT_FP_EQ(0x1.0p-40, LIBC_NAMESPACE::asinh(0x1.0p-40));
  EXPECT_FP_EQ(-0x1.0p-40, LIBC_NAMESPACE::asinh(-0x1.0p-40));
  EXPECT_FP_EQ(min_denormal, LIBC_NAMESPACE::asinh(min_denormal));
}

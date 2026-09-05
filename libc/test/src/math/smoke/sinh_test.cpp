//===-- Unittests for sinh ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/math_macros.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/math/sinh.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcSinhTest = LIBC_NAMESPACE::testing::FPTest<double>;

TEST_F(LlvmLibcSinhTest, SpecialNumbers) {
  EXPECT_FP_EQ_WITH_EXCEPTION(aNaN, LIBC_NAMESPACE::sinh(sNaN), FE_INVALID);
  EXPECT_FP_EQ(aNaN, LIBC_NAMESPACE::sinh(aNaN));
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::sinh(0.0));
  EXPECT_FP_EQ(-0.0, LIBC_NAMESPACE::sinh(-0.0));
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::sinh(inf));
  EXPECT_FP_EQ(neg_inf, LIBC_NAMESPACE::sinh(neg_inf));
}

// sinh is odd, and small enough arguments come straight back.
TEST_F(LlvmLibcSinhTest, SmallValues) {
  EXPECT_FP_EQ(0x1.0p-40, LIBC_NAMESPACE::sinh(0x1.0p-40));
  EXPECT_FP_EQ(-0x1.0p-40, LIBC_NAMESPACE::sinh(-0x1.0p-40));
  EXPECT_FP_EQ(min_denormal, LIBC_NAMESPACE::sinh(min_denormal));
}

// Past the top of the range the answer will not fit.
TEST_F(LlvmLibcSinhTest, Overflow) {
  LIBC_NAMESPACE::libc_errno = 0;
  EXPECT_FP_EQ_WITH_EXCEPTION(inf, LIBC_NAMESPACE::sinh(0x1.0p+10),
                              FE_OVERFLOW);
  EXPECT_MATH_ERRNO(ERANGE);
  LIBC_NAMESPACE::libc_errno = 0;
  EXPECT_FP_EQ_WITH_EXCEPTION(neg_inf, LIBC_NAMESPACE::sinh(-0x1.0p+10),
                              FE_OVERFLOW);
  EXPECT_MATH_ERRNO(ERANGE);
}

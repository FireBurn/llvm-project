//===-- Unittests for atanh ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/math_macros.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/math/atanh.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcAtanhTest = LIBC_NAMESPACE::testing::FPTest<double>;

TEST_F(LlvmLibcAtanhTest, SpecialNumbers) {
  EXPECT_FP_EQ_WITH_EXCEPTION(aNaN, LIBC_NAMESPACE::atanh(sNaN), FE_INVALID);
  EXPECT_FP_EQ(aNaN, LIBC_NAMESPACE::atanh(aNaN));
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::atanh(0.0));
  EXPECT_FP_EQ(-0.0, LIBC_NAMESPACE::atanh(-0.0));
}

TEST_F(LlvmLibcAtanhTest, SmallValues) {
  EXPECT_FP_EQ(0x1.0p-40, LIBC_NAMESPACE::atanh(0x1.0p-40));
  EXPECT_FP_EQ(-0x1.0p-40, LIBC_NAMESPACE::atanh(-0x1.0p-40));
  EXPECT_FP_EQ(min_denormal, LIBC_NAMESPACE::atanh(min_denormal));
}

// atanh runs out to infinity at one and is not defined past it.
TEST_F(LlvmLibcAtanhTest, Poles) {
  LIBC_NAMESPACE::libc_errno = 0;
  EXPECT_FP_EQ_WITH_EXCEPTION(inf, LIBC_NAMESPACE::atanh(1.0), FE_DIVBYZERO);
  EXPECT_MATH_ERRNO(ERANGE);
  LIBC_NAMESPACE::libc_errno = 0;
  EXPECT_FP_EQ_WITH_EXCEPTION(neg_inf, LIBC_NAMESPACE::atanh(-1.0),
                              FE_DIVBYZERO);
  EXPECT_MATH_ERRNO(ERANGE);
}

TEST_F(LlvmLibcAtanhTest, OutOfDomain) {
  LIBC_NAMESPACE::libc_errno = 0;
  EXPECT_FP_IS_NAN_WITH_EXCEPTION(LIBC_NAMESPACE::atanh(2.0), FE_INVALID);
  EXPECT_MATH_ERRNO(EDOM);
  LIBC_NAMESPACE::libc_errno = 0;
  EXPECT_FP_IS_NAN_WITH_EXCEPTION(LIBC_NAMESPACE::atanh(inf), FE_INVALID);
  EXPECT_MATH_ERRNO(EDOM);
}

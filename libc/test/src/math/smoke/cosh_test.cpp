//===-- Unittests for cosh ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/math_macros.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/math/cosh.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcCoshTest = LIBC_NAMESPACE::testing::FPTest<double>;

TEST_F(LlvmLibcCoshTest, SpecialNumbers) {
  EXPECT_FP_EQ_WITH_EXCEPTION(aNaN, LIBC_NAMESPACE::cosh(sNaN), FE_INVALID);
  EXPECT_FP_EQ(aNaN, LIBC_NAMESPACE::cosh(aNaN));
  // cosh is even and never below one.
  EXPECT_FP_EQ(1.0, LIBC_NAMESPACE::cosh(0.0));
  EXPECT_FP_EQ(1.0, LIBC_NAMESPACE::cosh(-0.0));
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::cosh(inf));
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::cosh(neg_inf));
}

TEST_F(LlvmLibcCoshTest, SmallValues) {
  EXPECT_FP_EQ(1.0, LIBC_NAMESPACE::cosh(0x1.0p-40));
  EXPECT_FP_EQ(1.0, LIBC_NAMESPACE::cosh(-0x1.0p-40));
}

TEST_F(LlvmLibcCoshTest, Overflow) {
  LIBC_NAMESPACE::libc_errno = 0;
  EXPECT_FP_EQ_WITH_EXCEPTION(inf, LIBC_NAMESPACE::cosh(0x1.0p+10),
                              FE_OVERFLOW);
  EXPECT_MATH_ERRNO(ERANGE);
  LIBC_NAMESPACE::libc_errno = 0;
  EXPECT_FP_EQ_WITH_EXCEPTION(inf, LIBC_NAMESPACE::cosh(-0x1.0p+10),
                              FE_OVERFLOW);
  EXPECT_MATH_ERRNO(ERANGE);
}

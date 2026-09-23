//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for lgamma and lgamma_r
///
//===----------------------------------------------------------------------===//

#include "hdr/math_macros.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/math/lgamma.h"
#include "src/math/lgamma_r.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcLgammaTest = LIBC_NAMESPACE::testing::FPTest<double>;

TEST_F(LlvmLibcLgammaTest, SpecialNumbers) {
  EXPECT_FP_EQ(aNaN, LIBC_NAMESPACE::lgamma(aNaN));
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::lgamma(inf));
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::lgamma(neg_inf));
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::lgamma(0.0));
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::lgamma(-0.0));
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::lgamma(-4.0));
}

TEST_F(LlvmLibcLgammaTest, KnownValues) {
  // gamma is one at one and two, so its logarithm is zero.
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::lgamma(1.0));
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::lgamma(2.0));
  // gamma(3) = 2 and gamma(5) = 24.
  EXPECT_TRUE(LIBC_NAMESPACE::lgamma(3.0) > 0.693147180559945);
  EXPECT_TRUE(LIBC_NAMESPACE::lgamma(3.0) < 0.693147180559946);
  EXPECT_TRUE(LIBC_NAMESPACE::lgamma(5.0) > 3.178053830347945);
  EXPECT_TRUE(LIBC_NAMESPACE::lgamma(5.0) < 3.178053830347946);
}

// The logarithm is of the absolute value, so a negative gamma gives the same
// as its magnitude and the sign comes out separately.
TEST_F(LlvmLibcLgammaTest, SignComesOutOfLgammaR) {
  int sign = 0;
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::lgamma_r(1.0, &sign));
  EXPECT_EQ(sign, 1);

  // gamma(-1/2) is negative, gamma(-3/2) positive.
  double half = LIBC_NAMESPACE::lgamma_r(-0.5, &sign);
  EXPECT_EQ(sign, -1);
  EXPECT_TRUE(half > 1.265512123484645);
  EXPECT_TRUE(half < 1.265512123484646);
  LIBC_NAMESPACE::lgamma_r(-1.5, &sign);
  EXPECT_EQ(sign, 1);
}

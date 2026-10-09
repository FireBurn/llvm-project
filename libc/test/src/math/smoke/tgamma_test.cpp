//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for tgamma and tgammaf
///
//===----------------------------------------------------------------------===//

#include "hdr/math_macros.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/math/tgamma.h"
#include "src/math/tgammaf.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcTgammaTest = LIBC_NAMESPACE::testing::FPTest<double>;
using LlvmLibcTgammafTest = LIBC_NAMESPACE::testing::FPTest<float>;

TEST_F(LlvmLibcTgammaTest, SpecialNumbers) {
  EXPECT_FP_EQ(aNaN, LIBC_NAMESPACE::tgamma(aNaN));
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::tgamma(inf));
  EXPECT_FP_IS_NAN(LIBC_NAMESPACE::tgamma(neg_inf));
  // The gamma function has a pole at zero, approached from either side.
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::tgamma(0.0));
  EXPECT_FP_EQ(neg_inf, LIBC_NAMESPACE::tgamma(-0.0));
  // and at every negative integer.
  EXPECT_FP_IS_NAN(LIBC_NAMESPACE::tgamma(-3.0));
}

// For a positive integer n it is (n-1)!, which a good implementation gets
// exactly.
TEST_F(LlvmLibcTgammaTest, Factorials) {
  EXPECT_FP_EQ(1.0, LIBC_NAMESPACE::tgamma(1.0));
  EXPECT_FP_EQ(1.0, LIBC_NAMESPACE::tgamma(2.0));
  EXPECT_FP_EQ(2.0, LIBC_NAMESPACE::tgamma(3.0));
  EXPECT_FP_EQ(24.0, LIBC_NAMESPACE::tgamma(5.0));
  EXPECT_FP_EQ(362880.0, LIBC_NAMESPACE::tgamma(10.0));
}

static bool near(double got, double want, double tolerance) {
  double d = got - want;
  return (d < 0 ? -d : d) <= tolerance;
}

TEST_F(LlvmLibcTgammaTest, HalfIntegers) {
  // gamma(1/2) is the square root of pi.
  EXPECT_TRUE(near(LIBC_NAMESPACE::tgamma(0.5), 1.772453850905516, 3e-16));
  // Reflection reaches the negative side: gamma(-1/2) = -2 sqrt(pi).
  EXPECT_TRUE(near(LIBC_NAMESPACE::tgamma(-0.5), -3.544907701811032, 6e-16));
}

// Past the range, the result is infinity or a zero carrying the sign gamma
// has there: negative on (-201, -200) and positive on (-200, -199).
TEST_F(LlvmLibcTgammaTest, OverflowAndUnderflow) {
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::tgamma(200.0));
  EXPECT_FP_EQ(-0.0, LIBC_NAMESPACE::tgamma(-200.5));
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::tgamma(-199.5));
}

TEST_F(LlvmLibcTgammafTest, SpecialNumbers) {
  EXPECT_FP_EQ(aNaN, LIBC_NAMESPACE::tgammaf(aNaN));
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::tgammaf(inf));
  EXPECT_FP_IS_NAN(LIBC_NAMESPACE::tgammaf(neg_inf));
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::tgammaf(0.0f));
  EXPECT_FP_EQ(neg_inf, LIBC_NAMESPACE::tgammaf(-0.0f));
  EXPECT_FP_IS_NAN(LIBC_NAMESPACE::tgammaf(-3.0f));
}

TEST_F(LlvmLibcTgammafTest, Factorials) {
  EXPECT_FP_EQ(1.0f, LIBC_NAMESPACE::tgammaf(1.0f));
  EXPECT_FP_EQ(6.0f, LIBC_NAMESPACE::tgammaf(4.0f));
  EXPECT_FP_EQ(120.0f, LIBC_NAMESPACE::tgammaf(6.0f));
}

TEST_F(LlvmLibcTgammafTest, Overflow) {
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::tgammaf(100.0f));
}

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the Bessel functions j0, j1, jn, y0, y1 and yn
///
//===----------------------------------------------------------------------===//

#include "hdr/math_macros.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/math/j0.h"
#include "src/math/j0f.h"
#include "src/math/j1.h"
#include "src/math/j1f.h"
#include "src/math/jn.h"
#include "src/math/jnf.h"
#include "src/math/y0.h"
#include "src/math/y0f.h"
#include "src/math/y1.h"
#include "src/math/y1f.h"
#include "src/math/yn.h"
#include "src/math/ynf.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcBesselTest = LIBC_NAMESPACE::testing::FPTest<double>;
using LlvmLibcBesselfTest = LIBC_NAMESPACE::testing::FPTest<float>;

static bool near(double got, double want, double tolerance = 1e-12) {
  double d = got - want;
  return (d < 0 ? -d : d) <= tolerance;
}

TEST_F(LlvmLibcBesselTest, SpecialNumbers) {
  EXPECT_FP_EQ(aNaN, LIBC_NAMESPACE::j0(aNaN));
  EXPECT_FP_EQ(aNaN, LIBC_NAMESPACE::y0(aNaN));
  EXPECT_FP_EQ(1.0, LIBC_NAMESPACE::j0(0.0));
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::j1(0.0));
  // The first order functions are zero at infinity and the second order
  // ones blow up at zero.
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::j0(inf));
  EXPECT_FP_EQ(neg_inf, LIBC_NAMESPACE::y0(0.0));
  EXPECT_FP_EQ(neg_inf, LIBC_NAMESPACE::y1(0.0));
  EXPECT_FP_IS_NAN(LIBC_NAMESPACE::y0(-1.0));
}

TEST_F(LlvmLibcBesselTest, KnownValues) {
  EXPECT_TRUE(near(LIBC_NAMESPACE::j0(1.0), 0.7651976865579666));
  EXPECT_TRUE(near(LIBC_NAMESPACE::j1(1.0), 0.4400505857449335));
  EXPECT_TRUE(near(LIBC_NAMESPACE::y0(1.0), 0.08825696421567697));
  EXPECT_TRUE(near(LIBC_NAMESPACE::y1(1.0), -0.7812128213002887));
  EXPECT_TRUE(near(LIBC_NAMESPACE::j0(10.0), -0.2459357644513483));
}

TEST_F(LlvmLibcBesselTest, OrdersAgreeWithTheDedicatedFunctions) {
  EXPECT_FP_EQ(LIBC_NAMESPACE::j0(3.5), LIBC_NAMESPACE::jn(0, 3.5));
  EXPECT_FP_EQ(LIBC_NAMESPACE::j1(3.5), LIBC_NAMESPACE::jn(1, 3.5));
  EXPECT_FP_EQ(LIBC_NAMESPACE::y0(3.5), LIBC_NAMESPACE::yn(0, 3.5));
  EXPECT_FP_EQ(LIBC_NAMESPACE::y1(3.5), LIBC_NAMESPACE::yn(1, 3.5));
}

TEST_F(LlvmLibcBesselTest, HigherOrders) {
  // The recurrence J(n+1) = 2n/x J(n) - J(n-1) ties the orders together.
  double x = 2.5;
  double j1 = LIBC_NAMESPACE::jn(1, x);
  double j2 = LIBC_NAMESPACE::jn(2, x);
  double j3 = LIBC_NAMESPACE::jn(3, x);
  EXPECT_TRUE(near(j3, 2.0 * 2.0 / x * j2 - j1));
  double y1 = LIBC_NAMESPACE::yn(1, x);
  double y2 = LIBC_NAMESPACE::yn(2, x);
  double y3 = LIBC_NAMESPACE::yn(3, x);
  EXPECT_TRUE(near(y3, 2.0 * 2.0 / x * y2 - y1));
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::jn(5, 0.0));
}

// A negative order reflects: J(-n, x) = (-1)^n J(n, x).
TEST_F(LlvmLibcBesselTest, NegativeOrders) {
  EXPECT_FP_EQ(-LIBC_NAMESPACE::jn(1, 2.0), LIBC_NAMESPACE::jn(-1, 2.0));
  EXPECT_FP_EQ(LIBC_NAMESPACE::jn(2, 2.0), LIBC_NAMESPACE::jn(-2, 2.0));
}

TEST_F(LlvmLibcBesselfTest, SingleFunctionsAgreeWithKnownValues) {
  EXPECT_FP_EQ(1.0f, LIBC_NAMESPACE::j0f(0.0f));
  EXPECT_FP_EQ(0.0f, LIBC_NAMESPACE::j1f(0.0f));
  EXPECT_FP_EQ(neg_inf, LIBC_NAMESPACE::y0f(0.0f));
  EXPECT_TRUE(near(LIBC_NAMESPACE::j0f(1.0f), 0.7651976865579666, 1e-6));
  EXPECT_TRUE(near(LIBC_NAMESPACE::j1f(1.0f), 0.4400505857449335, 1e-6));
  EXPECT_TRUE(near(LIBC_NAMESPACE::y0f(1.0f), 0.08825696421567697, 1e-6));
  EXPECT_TRUE(near(LIBC_NAMESPACE::y1f(1.0f), -0.7812128213002887, 1e-6));
  EXPECT_FP_EQ(LIBC_NAMESPACE::j1f(2.0f), LIBC_NAMESPACE::jnf(1, 2.0f));
  EXPECT_FP_EQ(LIBC_NAMESPACE::y1f(2.0f), LIBC_NAMESPACE::ynf(1, 2.0f));
}

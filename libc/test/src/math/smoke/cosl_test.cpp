//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for cosl
///
//===----------------------------------------------------------------------===//

#include "src/math/cosl.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcCoslTest = LIBC_NAMESPACE::testing::FPTest<long double>;

namespace {
constexpr long double PI = 3.14159265358979323846264338327950288L;
constexpr long double EPS = 4e-19L;

bool near(long double expected, long double actual) {
  long double d = expected - actual;
  return (d < 0 ? -d : d) <= EPS;
}
} // namespace

TEST_F(LlvmLibcCoslTest, SpecialNumbers) {
  EXPECT_FP_EQ_WITH_EXCEPTION(aNaN, LIBC_NAMESPACE::cosl(sNaN), FE_INVALID);
  EXPECT_MATH_ERRNO(0);

  EXPECT_FP_EQ_ALL_ROUNDING(aNaN, LIBC_NAMESPACE::cosl(aNaN));
  EXPECT_FP_EQ_ALL_ROUNDING(aNaN, LIBC_NAMESPACE::cosl(inf));
  EXPECT_MATH_ERRNO(EDOM);
  EXPECT_FP_EQ_ALL_ROUNDING(aNaN, LIBC_NAMESPACE::cosl(neg_inf));
  EXPECT_FP_EQ_ALL_ROUNDING(1.0L, LIBC_NAMESPACE::cosl(zero));
}

TEST_F(LlvmLibcCoslTest, KnownValues) {
  EXPECT_TRUE(near(0.0L, LIBC_NAMESPACE::cosl(PI / 2)));
  EXPECT_TRUE(near(0.5L, LIBC_NAMESPACE::cosl(PI / 3)));
  EXPECT_TRUE(near(-1.0L, LIBC_NAMESPACE::cosl(PI)));
}

TEST_F(LlvmLibcCoslTest, ReferenceValues) {
  // Arguments up to the largest exponent, results from an exact computation.
  static const struct {
    long double x, expected;
  } cases[] = {
      {0x1.0p10L, 0x1.f98669d7aedb7816p-1L},
      {0x1.921fb54442d1846ap0L, -0x1.d9cceba3f91f1976p-66L},
      {0x1.0p62L, -0x1.6c2b1f820c707c0ap-1L},
      {0x1.0p63L, 0x1.82aa375b3c33e706p-7L},
      {0x1.fedcba9876543210p70L, -0x1.fd9bbccb9fe4baf6p-1L},
      {0x1.23456789abcdef0p100L, -0x1.571a2a22b44e9b9ep-1L},
      {0x1.0p400L, -0x1.fbcceec896f7563cp-1L},
      {0x1.8p3000L, -0x1.fc7d9819d4bce610p-1L},
      {0x1.5p16000L, 0x1.2affa12a046a4334p-1L},
      {0x1.0p16383L, 0x1.d7985f9055c73d80p-1L},
  };
  for (const auto &c : cases) {
    long double actual = LIBC_NAMESPACE::cosl(c.x);
    long double err = actual - c.expected;
    long double mag = c.expected < 0 ? -c.expected : c.expected;
    EXPECT_TRUE((err < 0 ? -err : err) <= 4 * 0x1.0p-63L * mag);
    err = LIBC_NAMESPACE::cosl(-c.x) - c.expected;
    EXPECT_TRUE((err < 0 ? -err : err) <= 4 * 0x1.0p-63L * mag);
  }
}

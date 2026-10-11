//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for sinl
///
//===----------------------------------------------------------------------===//

#include "src/math/sinl.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcSinlTest = LIBC_NAMESPACE::testing::FPTest<long double>;

namespace {
constexpr long double PI = 3.14159265358979323846264338327950288L;
constexpr long double EPS = 4e-19L;

bool near(long double expected, long double actual) {
  long double d = expected - actual;
  return (d < 0 ? -d : d) <= EPS;
}
} // namespace

TEST_F(LlvmLibcSinlTest, SpecialNumbers) {
  EXPECT_FP_EQ_WITH_EXCEPTION(aNaN, LIBC_NAMESPACE::sinl(sNaN), FE_INVALID);
  EXPECT_MATH_ERRNO(0);

  EXPECT_FP_EQ_ALL_ROUNDING(aNaN, LIBC_NAMESPACE::sinl(aNaN));
  EXPECT_FP_EQ_ALL_ROUNDING(aNaN, LIBC_NAMESPACE::sinl(inf));
  EXPECT_MATH_ERRNO(EDOM);
  EXPECT_FP_EQ_ALL_ROUNDING(aNaN, LIBC_NAMESPACE::sinl(neg_inf));
  EXPECT_FP_EQ_ALL_ROUNDING(zero, LIBC_NAMESPACE::sinl(zero));
  EXPECT_FP_EQ_ALL_ROUNDING(neg_zero, LIBC_NAMESPACE::sinl(neg_zero));
}

TEST_F(LlvmLibcSinlTest, KnownValues) {
  EXPECT_TRUE(near(1.0L, LIBC_NAMESPACE::sinl(PI / 2)));
  EXPECT_TRUE(near(0.5L, LIBC_NAMESPACE::sinl(PI / 6)));
  EXPECT_TRUE(near(-1.0L, LIBC_NAMESPACE::sinl(-PI / 2)));
  EXPECT_TRUE(near(0.0L, LIBC_NAMESPACE::sinl(PI)));
}

TEST_F(LlvmLibcSinlTest, ReferenceValues) {
  // Arguments up to the largest exponent, results from an exact computation.
  static const struct {
    long double x, expected;
  } cases[] = {
      {0x1.0p10L, -0x1.44ad2614e80ab75ap-3L},
      {0x1.921fb54442d1846ap0L, 0x1.0000000000000000p0L},
      {0x1.0p62L, -0x1.67e57355f014728cp-1L},
      {0x1.0p63L, 0x1.fff6dfd42dc54430p-1L},
      {0x1.fedcba9876543210p70L, 0x1.8b70bf4f6fbbe1acp-4L},
      {0x1.23456789abcdef0p100L, -0x1.7c08610ee41525c6p-1L},
      {0x1.0p400L, 0x1.05c46f94edf3d294p-3L},
      {0x1.8p3000L, -0x1.dec091268e03c5aep-4L},
      {0x1.5p16000L, -0x1.9f9fea4c21c9ee28p-1L},
      {0x1.0p16383L, 0x1.8eb52c84db31a208p-2L},
  };
  for (const auto &c : cases) {
    long double actual = LIBC_NAMESPACE::sinl(c.x);
    long double err = actual - c.expected;
    long double mag = c.expected < 0 ? -c.expected : c.expected;
    EXPECT_TRUE((err < 0 ? -err : err) <= 4 * 0x1.0p-63L * mag);
    err = LIBC_NAMESPACE::sinl(-c.x) + c.expected;
    EXPECT_TRUE((err < 0 ? -err : err) <= 4 * 0x1.0p-63L * mag);
  }
}

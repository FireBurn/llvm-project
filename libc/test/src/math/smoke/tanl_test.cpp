//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for tanl
///
//===----------------------------------------------------------------------===//

#include "src/math/tanl.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcTanlTest = LIBC_NAMESPACE::testing::FPTest<long double>;

namespace {
constexpr long double PI = 3.14159265358979323846264338327950288L;
constexpr long double EPS = 4e-19L;

bool near(long double expected, long double actual) {
  long double d = expected - actual;
  return (d < 0 ? -d : d) <= EPS;
}
} // namespace

TEST_F(LlvmLibcTanlTest, SpecialNumbers) {
  EXPECT_FP_EQ_WITH_EXCEPTION(aNaN, LIBC_NAMESPACE::tanl(sNaN), FE_INVALID);
  EXPECT_MATH_ERRNO(0);

  EXPECT_FP_EQ_ALL_ROUNDING(aNaN, LIBC_NAMESPACE::tanl(aNaN));
  EXPECT_FP_EQ_ALL_ROUNDING(aNaN, LIBC_NAMESPACE::tanl(inf));
  EXPECT_MATH_ERRNO(EDOM);
  EXPECT_FP_EQ_ALL_ROUNDING(aNaN, LIBC_NAMESPACE::tanl(neg_inf));
  EXPECT_FP_EQ_ALL_ROUNDING(zero, LIBC_NAMESPACE::tanl(zero));
  EXPECT_FP_EQ_ALL_ROUNDING(neg_zero, LIBC_NAMESPACE::tanl(neg_zero));
}

TEST_F(LlvmLibcTanlTest, KnownValues) {
  EXPECT_TRUE(near(1.0L, LIBC_NAMESPACE::tanl(PI / 4)));
  EXPECT_TRUE(near(-1.0L, LIBC_NAMESPACE::tanl(-PI / 4)));
  EXPECT_TRUE(near(0.0L, LIBC_NAMESPACE::tanl(PI)));
}

TEST_F(LlvmLibcTanlTest, ReferenceValues) {
  // Arguments up to the largest exponent, results from an exact computation.
  static const struct {
    long double x, expected;
  } cases[] = {
      {0x1.0p10L, -0x1.48d5be43ada0180ap-3L},
      {0x1.921fb54442d1846ap0L, -0x1.14a3c09b557b46bep65L},
      {0x1.0p62L, 0x1.f9fe5be664fa7412p-1L},
      {0x1.0p63L, 0x1.52f50e757941cc00p6L},
      {0x1.fedcba9876543210p70L, -0x1.8d4bd81dd4730f22p-4L},
      {0x1.23456789abcdef0p100L, 0x1.1b8e1ad5c3d470d0p0L},
      {0x1.0p400L, -0x1.07ee9ff9cd87bcc8p-3L},
      {0x1.8p3000L, 0x1.e20e6e4897bc6fe2p-4L},
      {0x1.5p16000L, -0x1.63dab2318ee56c24p0L},
      {0x1.0p16383L, 0x1.b0de23a176a6f93ep-2L},
  };
  for (const auto &c : cases) {
    long double actual = LIBC_NAMESPACE::tanl(c.x);
    long double err = actual - c.expected;
    long double mag = c.expected < 0 ? -c.expected : c.expected;
    EXPECT_TRUE((err < 0 ? -err : err) <= 4 * 0x1.0p-63L * mag);
    err = LIBC_NAMESPACE::tanl(-c.x) + c.expected;
    EXPECT_TRUE((err < 0 ? -err : err) <= 4 * 0x1.0p-63L * mag);
  }
}

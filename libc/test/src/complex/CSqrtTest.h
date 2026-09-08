//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains utility class to test different flavors of csqrt.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TEST_SRC_COMPLEX_CSQRTTEST_H
#define LLVM_LIBC_TEST_SRC_COMPLEX_CSQRTTEST_H

#include "test/UnitTest/FEnvSafeTest.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

#include "hdr/math_macros.h"

#include "src/__support/FPUtil/ManipulationFunctions.h"
#include "src/__support/FPUtil/sqrt.h"

template <typename CFPT, typename FPT>
class CSqrtTest : public LIBC_NAMESPACE::testing::FEnvSafeTest {

  DECLARE_SPECIAL_CONSTANTS(FPT)

public:
  using CSqrtFunc = CFPT (*)(CFPT);

  // The root is the principal one, so its real part is never negative and
  // its imaginary part carries the sign of the argument's.
  void testZeroValues(CSqrtFunc func) {
    EXPECT_FP_EQ(zero, __real__ func(CFPT{0.0, 0.0}));
    EXPECT_FP_EQ(zero, __imag__ func(CFPT{0.0, 0.0}));
    EXPECT_FP_EQ(zero, __real__ func(CFPT{-0.0, 0.0}));
    EXPECT_FP_EQ(zero, __imag__ func(CFPT{-0.0, 0.0}));
    // A negative zero imaginary part is carried through.
    EXPECT_FP_EQ(zero, __real__ func(CFPT{0.0, -0.0}));
    EXPECT_FP_EQ(neg_zero, __imag__ func(CFPT{0.0, -0.0}));
  }

  void testBasicValues(CSqrtFunc func) {
    // sqrt(4) = 2
    EXPECT_FP_EQ(FPT(2.0), __real__ func(CFPT{4.0, 0.0}));
    EXPECT_FP_EQ(zero, __imag__ func(CFPT{4.0, 0.0}));
    // sqrt(-4) = 2i, and the cut is taken from above.
    EXPECT_FP_EQ(zero, __real__ func(CFPT{-4.0, 0.0}));
    EXPECT_FP_EQ(FPT(2.0), __imag__ func(CFPT{-4.0, 0.0}));
    // Below the cut it is -2i.
    EXPECT_FP_EQ(zero, __real__ func(CFPT{-4.0, -0.0}));
    EXPECT_FP_EQ(FPT(-2.0), __imag__ func(CFPT{-4.0, -0.0}));
    // sqrt(-1) = i
    EXPECT_FP_EQ(zero, __real__ func(CFPT{-1.0, 0.0}));
    EXPECT_FP_EQ(FPT(1.0), __imag__ func(CFPT{-1.0, 0.0}));
    // (3 + 4i) is the square of (2 + i).
    EXPECT_FP_EQ(FPT(2.0), __real__ func(CFPT{3.0, 4.0}));
    EXPECT_FP_EQ(FPT(1.0), __imag__ func(CFPT{3.0, 4.0}));
    // and (-3 + 4i) the square of (1 + 2i).
    EXPECT_FP_EQ(FPT(1.0), __real__ func(CFPT{-3.0, 4.0}));
    EXPECT_FP_EQ(FPT(2.0), __imag__ func(CFPT{-3.0, 4.0}));
    // (-3 - 4i) is the square of (1 - 2i).
    EXPECT_FP_EQ(FPT(1.0), __real__ func(CFPT{-3.0, -4.0}));
    EXPECT_FP_EQ(FPT(-2.0), __imag__ func(CFPT{-3.0, -4.0}));
  }

  // An infinite imaginary part settles the answer whatever the real part is,
  // even where that real part is not a number.
  void testInfinityValues(CSqrtFunc func) {
    EXPECT_FP_EQ(inf, __real__ func(CFPT{1.0, inf}));
    EXPECT_FP_EQ(inf, __imag__ func(CFPT{1.0, inf}));
    EXPECT_FP_EQ(inf, __real__ func(CFPT{aNaN, inf}));
    EXPECT_FP_EQ(inf, __imag__ func(CFPT{aNaN, inf}));
    EXPECT_FP_EQ(inf, __real__ func(CFPT{neg_inf, inf}));
    EXPECT_FP_EQ(inf, __imag__ func(CFPT{neg_inf, inf}));
    EXPECT_FP_EQ(inf, __real__ func(CFPT{1.0, neg_inf}));
    EXPECT_FP_EQ(neg_inf, __imag__ func(CFPT{1.0, neg_inf}));

    // A positive infinity along the real axis stays on it.
    EXPECT_FP_EQ(inf, __real__ func(CFPT{inf, 1.0}));
    EXPECT_FP_EQ(zero, __imag__ func(CFPT{inf, 1.0}));
    EXPECT_FP_EQ(inf, __real__ func(CFPT{inf, -1.0}));
    EXPECT_FP_EQ(neg_zero, __imag__ func(CFPT{inf, -1.0}));

    // A negative one turns onto the imaginary axis.
    EXPECT_FP_EQ(zero, __real__ func(CFPT{neg_inf, 1.0}));
    EXPECT_FP_EQ(inf, __imag__ func(CFPT{neg_inf, 1.0}));
    EXPECT_FP_EQ(zero, __real__ func(CFPT{neg_inf, -1.0}));
    EXPECT_FP_EQ(neg_inf, __imag__ func(CFPT{neg_inf, -1.0}));
  }

  void testNaNValues(CSqrtFunc func) {
    EXPECT_FP_EQ(aNaN, __real__ func(CFPT{aNaN, 1.0}));
    EXPECT_FP_EQ(aNaN, __imag__ func(CFPT{aNaN, 1.0}));
    EXPECT_FP_EQ(aNaN, __real__ func(CFPT{1.0, aNaN}));
    EXPECT_FP_EQ(aNaN, __imag__ func(CFPT{1.0, aNaN}));
    EXPECT_FP_EQ(aNaN, __real__ func(CFPT{aNaN, aNaN}));
    EXPECT_FP_EQ(aNaN, __imag__ func(CFPT{aNaN, aNaN}));
  }

  // Squaring the root gives the argument back. The real part is not checked
  // directly: recovering it as re^2 - im^2 subtracts two numbers that are
  // close together wherever the argument lies near the imaginary axis, and
  // loses every digit. The modulus and the imaginary part between them pin
  // the root down, and neither is worked out by such a subtraction.
  void testRoundTrip(CSqrtFunc func) {
    constexpr FPT VALUES[] = {FPT(1.0),  FPT(2.0),   FPT(0.5),
                              FPT(1e15), FPT(1e-15), FPT(7.25)};
    for (FPT a : VALUES) {
      for (FPT b : VALUES) {
        constexpr FPT SIGNS[] = {FPT(1.0), FPT(-1.0)};
        for (FPT sign : SIGNS) {
          const FPT re_in = a * sign;
          CFPT root = func(CFPT{re_in, b});
          FPT re = __real__ root;
          FPT im = __imag__ root;

          // The real part of the principal root is never negative, and the
          // imaginary part carries the sign of the argument's.
          EXPECT_TRUE(re >= FPT(0.0));
          EXPECT_TRUE(im >= FPT(0.0));

          // |root|^2 is |z|, and both are sums of squares rather than
          // differences, so nothing cancels.
          EXPECT_TRUE(close_enough(
              re * re + im * im,
              LIBC_NAMESPACE::fputil::sqrt<FPT>(re_in * re_in + b * b)));
          // 2 re im is the imaginary part of the square.
          EXPECT_TRUE(close_enough(FPT(2.0) * re * im, b));
        }
      }
    }
  }

private:
  // Within sixteen units in the last place of what was expected, which is
  // room enough for the rounding of the root and of squaring it again. The
  // comparison is made in the floating point type rather than on the bits,
  // since the x86 long double leaves the storage wider than the number.
  static bool close_enough(FPT got, FPT expected) {
    using FPBits = LIBC_NAMESPACE::fputil::FPBits<FPT>;
    if (got == expected)
      return true;
    const FPT difference = got > expected ? got - expected : expected - got;
    const FPT magnitude = FPBits(expected).abs().get_val();
    const FPT ulp =
        LIBC_NAMESPACE::fputil::ldexp(magnitude, -FPBits::FRACTION_LEN);
    return difference <= ulp * FPT(16.0);
  }
};

#define LIST_CSQRT_TESTS(U, T, func)                                           \
  using LlvmLibcCSqrtTest = CSqrtTest<U, T>;                                   \
  TEST_F(LlvmLibcCSqrtTest, Zero) { testZeroValues(&func); }                   \
  TEST_F(LlvmLibcCSqrtTest, Basic) { testBasicValues(&func); }                 \
  TEST_F(LlvmLibcCSqrtTest, Infinity) { testInfinityValues(&func); }           \
  TEST_F(LlvmLibcCSqrtTest, NaN) { testNaNValues(&func); }                     \
  TEST_F(LlvmLibcCSqrtTest, RoundTrip) { testRoundTrip(&func); }

#endif // LLVM_LIBC_TEST_SRC_COMPLEX_CSQRTTEST_H

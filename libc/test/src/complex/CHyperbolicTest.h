//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Utility class to test the complex trigonometric functions
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TEST_SRC_COMPLEX_CHYPERBOLICTEST_H
#define LLVM_LIBC_TEST_SRC_COMPLEX_CHYPERBOLICTEST_H

#include "test/UnitTest/FEnvSafeTest.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

#include "src/__support/FPUtil/BasicOperations.h"

// Shared by csin, ccos, ctan, csinh, ccosh and ctanh, in double and float.
// The reference values come from glibc, and the single precision functions
// are only held to what a float can show.
template <typename CFPT, typename FPT>
class CHyperbolicTest : public LIBC_NAMESPACE::testing::FEnvSafeTest {

  DECLARE_SPECIAL_CONSTANTS(FPT)

public:
  using CFunc = CFPT (*)(CFPT);

  static constexpr FPT TOLERANCE = sizeof(FPT) == 4 ? FPT(1e-5) : FPT(1e-13);

  void expectNear(FPT expected, FPT actual) {
    EXPECT_TRUE(LIBC_NAMESPACE::fputil::abs(actual - expected) <=
                TOLERANCE * LIBC_NAMESPACE::fputil::abs(expected));
  }

  // z is checked at 1 + 2i and 0.3 - 4i.
  void testValues(CFunc func, const double (&ref)[4]) {
    expectNear(FPT(ref[0]), __real__ func(CFPT{1.0, 2.0}));
    expectNear(FPT(ref[1]), __imag__ func(CFPT{1.0, 2.0}));
    expectNear(FPT(ref[2]), __real__ func(CFPT{0.3, -4.0}));
    expectNear(FPT(ref[3]), __imag__ func(CFPT{0.3, -4.0}));
  }

  // An argument of zero gives the zero or the one of the function, and keeps
  // the sign of a zero where there is one to keep.
  void testZero(CFunc func, double real, double imag) {
    EXPECT_FP_EQ(FPT(real), __real__ func(CFPT{0.0, 0.0}));
    EXPECT_FP_EQ(FPT(imag), __imag__ func(CFPT{0.0, 0.0}));
  }

  // The functions are odd or even, and take the conjugate of the argument to
  // the conjugate of the answer.
  void testConjugate(CFunc func) {
    EXPECT_FP_EQ(__real__ func(CFPT{1.0, 2.0}), __real__ func(CFPT{1.0, -2.0}));
    EXPECT_FP_EQ(__imag__ func(CFPT{1.0, 2.0}),
                 -(__imag__ func(CFPT{1.0, -2.0})));
  }

  void testNaN(CFunc func) {
    // A real argument with a NaN imaginary part has no answer.
    EXPECT_FP_IS_NAN(__real__ func(CFPT{1.0, aNaN}));
    EXPECT_FP_IS_NAN(__imag__ func(CFPT{1.0, aNaN}));
  }
};

#define LIST_CSIN_TESTS(U, T, func)                                              \
  using LlvmLibcCsinTest = CHyperbolicTest<U, T>;                                \
  TEST_F(LlvmLibcCsinTest, Values) {                                            \
    const double ref[4] = {3.165778513216168, 1.959601041421606, 8.07013461125557, -26.071053683632524};                                                \
    testValues(&func, ref);                                                    \
  }                                                                            \
  TEST_F(LlvmLibcCsinTest, Zero) { testZero(&func, 0.0, 0.0); }                   \
  TEST_F(LlvmLibcCsinTest, Conjugate) { testConjugate(&func); }                  \
  TEST_F(LlvmLibcCsinTest, NaN) { testNaN(&func); }

#define LIST_CCOS_TESTS(U, T, func)                                              \
  using LlvmLibcCcosTest = CHyperbolicTest<U, T>;                                \
  TEST_F(LlvmLibcCcosTest, Values) {                                            \
    const double ref[4] = {2.0327230070196656, -3.0518977991517997, 26.08855128178458, 8.064721969866037};                                                \
    testValues(&func, ref);                                                    \
  }                                                                            \
  TEST_F(LlvmLibcCcosTest, Zero) { testZero(&func, 1.0, -0.0); }                   \
  TEST_F(LlvmLibcCcosTest, Conjugate) { testConjugate(&func); }                  \
  TEST_F(LlvmLibcCcosTest, NaN) { testNaN(&func); }

#define LIST_CTAN_TESTS(U, T, func)                                              \
  using LlvmLibcCtanTest = CHyperbolicTest<U, T>;                                \
  TEST_F(LlvmLibcCtanTest, Values) {                                            \
    const double ref[4] = {0.0338128260798967, 1.0147936161466335, 0.00037862319504918576, -0.9994463430645638};                                                \
    testValues(&func, ref);                                                    \
  }                                                                            \
  TEST_F(LlvmLibcCtanTest, Zero) { testZero(&func, 0.0, 0.0); }                   \
  TEST_F(LlvmLibcCtanTest, Conjugate) { testConjugate(&func); }                  \
  TEST_F(LlvmLibcCtanTest, NaN) { testNaN(&func); }

#define LIST_CSINH_TESTS(U, T, func)                                              \
  using LlvmLibcCsinhTest = CHyperbolicTest<U, T>;                                \
  TEST_F(LlvmLibcCsinhTest, Values) {                                            \
    const double ref[4] = {-0.4890562590412937, 1.4031192506220405, -0.19904774723523994, 0.7911147959342035};                                                \
    testValues(&func, ref);                                                    \
  }                                                                            \
  TEST_F(LlvmLibcCsinhTest, Zero) { testZero(&func, 0.0, 0.0); }                   \
  TEST_F(LlvmLibcCsinhTest, Conjugate) { testConjugate(&func); }                  \
  TEST_F(LlvmLibcCsinhTest, NaN) { testNaN(&func); }

#define LIST_CCOSH_TESTS(U, T, func)                                              \
  using LlvmLibcCcoshTest = CHyperbolicTest<U, T>;                                \
  TEST_F(LlvmLibcCcoshTest, Values) {                                            \
    const double ref[4] = {-0.64214812471552, 1.0686074213827783, -0.6832788514033763, 0.23046171795270007};                                                \
    testValues(&func, ref);                                                    \
  }                                                                            \
  TEST_F(LlvmLibcCcoshTest, Zero) { testZero(&func, 1.0, 0.0); }                   \
  TEST_F(LlvmLibcCcoshTest, Conjugate) { testConjugate(&func); }                  \
  TEST_F(LlvmLibcCcoshTest, NaN) { testNaN(&func); }

#define LIST_CTANH_TESTS(U, T, func)                                              \
  using LlvmLibcCtanhTest = CHyperbolicTest<U, T>;                                \
  TEST_F(LlvmLibcCtanhTest, Values) {                                            \
    const double ref[4] = {1.16673625724092, -0.2434582011857253, 0.6121873998069954, -0.9513378538361051};                                                \
    testValues(&func, ref);                                                    \
  }                                                                            \
  TEST_F(LlvmLibcCtanhTest, Zero) { testZero(&func, 0.0, 0.0); }                   \
  TEST_F(LlvmLibcCtanhTest, Conjugate) { testConjugate(&func); }                  \
  TEST_F(LlvmLibcCtanhTest, NaN) { testNaN(&func); }

#endif // LLVM_LIBC_TEST_SRC_COMPLEX_CHYPERBOLICTEST_H

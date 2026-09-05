//===-- Unittests for erfc ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/math_macros.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/math/erfc.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcErfcTest = LIBC_NAMESPACE::testing::FPTest<double>;

TEST_F(LlvmLibcErfcTest, SpecialNumbers) {
  EXPECT_FP_EQ_WITH_EXCEPTION(aNaN, LIBC_NAMESPACE::erfc(sNaN), FE_INVALID);
  EXPECT_FP_EQ(aNaN, LIBC_NAMESPACE::erfc(aNaN));
  EXPECT_FP_EQ(1.0, LIBC_NAMESPACE::erfc(0.0));
  EXPECT_FP_EQ(1.0, LIBC_NAMESPACE::erfc(-0.0));
  // One less erf, so the tail runs out to nothing one way and to twice the
  // whole of it the other.
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::erfc(inf));
  EXPECT_FP_EQ(2.0, LIBC_NAMESPACE::erfc(neg_inf));
}

// Past the top of the range the answer is smaller than any there is.
TEST_F(LlvmLibcErfcTest, Underflow) {
  LIBC_NAMESPACE::libc_errno = 0;
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::erfc(40.0));
  EXPECT_MATH_ERRNO(ERANGE);
}

// It falls away steadily and never turns back.
TEST_F(LlvmLibcErfcTest, Decreasing) {
  double previous = LIBC_NAMESPACE::erfc(0.0);
  for (double x = 0.25; x < 26.0; x += 0.25) {
    const double value = LIBC_NAMESPACE::erfc(x);
    EXPECT_TRUE(value < previous);
    EXPECT_TRUE(value > 0.0);
    previous = value;
  }
}

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for powl
///
//===----------------------------------------------------------------------===//

#include "hdr/math_macros.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/math/powl.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcPowlTest = LIBC_NAMESPACE::testing::FPTest<long double>;

TEST_F(LlvmLibcPowlTest, SpecialNumbers) {
  EXPECT_FP_EQ(aNaN, LIBC_NAMESPACE::powl(aNaN, 2.0L));
  // Anything to the zero is one, even a NaN.
  EXPECT_FP_EQ(1.0L, LIBC_NAMESPACE::powl(aNaN, 0.0L));
  EXPECT_FP_EQ(1.0L, LIBC_NAMESPACE::powl(0.0L, 0.0L));
  EXPECT_FP_EQ(1.0L, LIBC_NAMESPACE::powl(1.0L, aNaN));
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::powl(0.0L, -1.0L));
  EXPECT_FP_EQ(0.0L, LIBC_NAMESPACE::powl(0.0L, 3.0L));
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::powl(inf, 2.0L));
  EXPECT_FP_EQ(0.0L, LIBC_NAMESPACE::powl(inf, -2.0L));
}

TEST_F(LlvmLibcPowlTest, IntegerPowers) {
  EXPECT_FP_EQ(1024.0L, LIBC_NAMESPACE::powl(2.0L, 10.0L));
  EXPECT_FP_EQ(0.125L, LIBC_NAMESPACE::powl(2.0L, -3.0L));
  EXPECT_FP_EQ(-8.0L, LIBC_NAMESPACE::powl(-2.0L, 3.0L));
  EXPECT_FP_EQ(16.0L, LIBC_NAMESPACE::powl(-2.0L, 4.0L));
}

TEST_F(LlvmLibcPowlTest, Roots) {
  EXPECT_TRUE(LIBC_NAMESPACE::powl(2.0L, 0.5L) > 1.41421356237309L);
  EXPECT_TRUE(LIBC_NAMESPACE::powl(2.0L, 0.5L) < 1.41421356237310L);
  EXPECT_FP_EQ(3.0L, LIBC_NAMESPACE::powl(9.0L, 0.5L));
}

TEST_F(LlvmLibcPowlTest, NegativeBaseWithFractionalExponentIsNaN) {
  EXPECT_FP_IS_NAN(LIBC_NAMESPACE::powl(-2.0L, 0.5L));
}

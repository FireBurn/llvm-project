//===-- Unittests for erfcf -------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for erfcf.
///
//===----------------------------------------------------------------------===//

#include "src/math/erfcf.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcErfcfTest = LIBC_NAMESPACE::testing::FPTest<float>;

TEST_F(LlvmLibcErfcfTest, SpecialNumbers) {
  EXPECT_FP_EQ_ALL_ROUNDING(aNaN, LIBC_NAMESPACE::erfcf(aNaN));
  EXPECT_FP_EQ_ALL_ROUNDING(zero, LIBC_NAMESPACE::erfcf(inf));
  EXPECT_FP_EQ_ALL_ROUNDING(2.0f, LIBC_NAMESPACE::erfcf(neg_inf));
  EXPECT_FP_EQ_ALL_ROUNDING(1.0f, LIBC_NAMESPACE::erfcf(zero));
  EXPECT_FP_EQ_ALL_ROUNDING(1.0f, LIBC_NAMESPACE::erfcf(neg_zero));
}

TEST_F(LlvmLibcErfcfTest, KnownValues) {
  // erfc(1) = 0.157299207..., erfc(-1) = 2 - erfc(1), and the tail is tiny.
  float one = LIBC_NAMESPACE::erfcf(1.0f) - 0.157299207f;
  EXPECT_TRUE(one < 1.0e-6f && one > -1.0e-6f);
  float minus_one = LIBC_NAMESPACE::erfcf(-1.0f) - 1.842700793f;
  EXPECT_TRUE(minus_one < 1.0e-6f && minus_one > -1.0e-6f);
  float tail = LIBC_NAMESPACE::erfcf(10.0f);
  EXPECT_TRUE(tail > 0.0f && tail < 1.0e-40f);
  EXPECT_FP_EQ_ALL_ROUNDING(zero, LIBC_NAMESPACE::erfcf(100.0f));
}

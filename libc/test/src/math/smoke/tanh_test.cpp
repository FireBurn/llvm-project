//===-- Unittests for tanh ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/math_macros.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/math/tanh.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcTanhTest = LIBC_NAMESPACE::testing::FPTest<double>;

TEST_F(LlvmLibcTanhTest, SpecialNumbers) {
  EXPECT_FP_EQ_WITH_EXCEPTION(aNaN, LIBC_NAMESPACE::tanh(sNaN), FE_INVALID);
  EXPECT_FP_EQ(aNaN, LIBC_NAMESPACE::tanh(aNaN));
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::tanh(0.0));
  EXPECT_FP_EQ(-0.0, LIBC_NAMESPACE::tanh(-0.0));
  // tanh runs up against one either way out without reaching it.
  EXPECT_FP_EQ(1.0, LIBC_NAMESPACE::tanh(inf));
  EXPECT_FP_EQ(-1.0, LIBC_NAMESPACE::tanh(neg_inf));
}

TEST_F(LlvmLibcTanhTest, SmallValues) {
  EXPECT_FP_EQ(0x1.0p-40, LIBC_NAMESPACE::tanh(0x1.0p-40));
  EXPECT_FP_EQ(-0x1.0p-40, LIBC_NAMESPACE::tanh(-0x1.0p-40));
  EXPECT_FP_EQ(min_denormal, LIBC_NAMESPACE::tanh(min_denormal));
}

// Far enough out nothing is nearer the answer than one, though tanh never
// reaches it.
TEST_F(LlvmLibcTanhTest, LargeValues) {
  EXPECT_FP_EQ_WITH_EXCEPTION(1.0, LIBC_NAMESPACE::tanh(30.0), FE_INEXACT);
  EXPECT_FP_EQ_WITH_EXCEPTION(-1.0, LIBC_NAMESPACE::tanh(-30.0), FE_INEXACT);
  EXPECT_TRUE(LIBC_NAMESPACE::tanh(0.5) < 1.0);
}

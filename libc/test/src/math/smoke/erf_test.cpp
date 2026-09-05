//===-- Unittests for erf ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/math_macros.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/math/erf.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcErfTest = LIBC_NAMESPACE::testing::FPTest<double>;

TEST_F(LlvmLibcErfTest, SpecialNumbers) {
  EXPECT_FP_EQ_WITH_EXCEPTION(aNaN, LIBC_NAMESPACE::erf(sNaN), FE_INVALID);
  EXPECT_FP_EQ(aNaN, LIBC_NAMESPACE::erf(aNaN));
  EXPECT_FP_EQ(0.0, LIBC_NAMESPACE::erf(0.0));
  EXPECT_FP_EQ(-0.0, LIBC_NAMESPACE::erf(-0.0));
  // erf runs up to one either way out.
  EXPECT_FP_EQ(1.0, LIBC_NAMESPACE::erf(inf));
  EXPECT_FP_EQ(-1.0, LIBC_NAMESPACE::erf(neg_inf));
}

// Near zero the answer is the argument times the slope there.
TEST_F(LlvmLibcErfTest, SmallValues) {
  EXPECT_FP_EQ(0x1.20dd750429b6dp-41, LIBC_NAMESPACE::erf(0x1.0p-41));
  EXPECT_FP_EQ(-0x1.20dd750429b6dp-41, LIBC_NAMESPACE::erf(-0x1.0p-41));
}

// Far enough out nothing is nearer the answer than one, though erf never
// reaches it.
TEST_F(LlvmLibcErfTest, LargeValues) {
  EXPECT_FP_EQ_WITH_EXCEPTION(1.0, LIBC_NAMESPACE::erf(10.0), FE_INEXACT);
  EXPECT_FP_EQ_WITH_EXCEPTION(-1.0, LIBC_NAMESPACE::erf(-10.0), FE_INEXACT);
  EXPECT_TRUE(LIBC_NAMESPACE::erf(1.0) < 1.0);
}

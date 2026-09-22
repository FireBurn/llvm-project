//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for log2l
///
//===----------------------------------------------------------------------===//

#include "hdr/math_macros.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/math/log2l.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcLog2lTest = LIBC_NAMESPACE::testing::FPTest<long double>;

TEST_F(LlvmLibcLog2lTest, SpecialNumbers) {
  EXPECT_FP_EQ(aNaN, LIBC_NAMESPACE::log2l(aNaN));
  EXPECT_FP_EQ(inf, LIBC_NAMESPACE::log2l(inf));
  EXPECT_FP_EQ(neg_inf, LIBC_NAMESPACE::log2l(0.0L));
  EXPECT_FP_EQ(neg_inf, LIBC_NAMESPACE::log2l(-0.0L));
  EXPECT_FP_IS_NAN(LIBC_NAMESPACE::log2l(-1.0L));
  EXPECT_FP_IS_NAN(LIBC_NAMESPACE::log2l(neg_inf));
}

// A power of two gives its exponent exactly.
TEST_F(LlvmLibcLog2lTest, PowersOfTwo) {
  EXPECT_FP_EQ(0.0L, LIBC_NAMESPACE::log2l(1.0L));
  EXPECT_FP_EQ(1.0L, LIBC_NAMESPACE::log2l(2.0L));
  EXPECT_FP_EQ(10.0L, LIBC_NAMESPACE::log2l(1024.0L));
  EXPECT_FP_EQ(-3.0L, LIBC_NAMESPACE::log2l(0.125L));
  EXPECT_FP_EQ(-1074.0L, LIBC_NAMESPACE::log2l(0x1.0p-1074L));
}

TEST_F(LlvmLibcLog2lTest, OtherValues) {
  EXPECT_TRUE(LIBC_NAMESPACE::log2l(10.0L) > 3.32192809488736L);
  EXPECT_TRUE(LIBC_NAMESPACE::log2l(10.0L) < 3.32192809488737L);
  EXPECT_TRUE(LIBC_NAMESPACE::log2l(3.0L) > 1.58496250072115L);
  EXPECT_TRUE(LIBC_NAMESPACE::log2l(3.0L) < 1.58496250072116L);
}

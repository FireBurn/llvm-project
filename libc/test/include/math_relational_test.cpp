//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the relational macros of math.h
///
//===----------------------------------------------------------------------===//

#include "include/math.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcMathRelationalTest, OrderedComparisons) {
  EXPECT_NE(static_cast<long long>(isgreater(2.0, 1.0)), 0LL);
  EXPECT_EQ(static_cast<long long>(isgreater(1.0, 2.0)), 0LL);
  EXPECT_NE(static_cast<long long>(isgreaterequal(2.0, 2.0)), 0LL);
  EXPECT_NE(static_cast<long long>(isless(1.0f, 2.0f)), 0LL);
  EXPECT_NE(static_cast<long long>(islessequal(2.0, 2.0)), 0LL);
  EXPECT_NE(static_cast<long long>(islessgreater(1.0, 2.0)), 0LL);
  EXPECT_EQ(static_cast<long long>(islessgreater(2.0, 2.0)), 0LL);
  EXPECT_EQ(static_cast<long long>(isunordered(1.0, 2.0)), 0LL);
}

// A NaN is unordered with everything, and none of the comparisons holds.
TEST(LlvmLibcMathRelationalTest, NaNIsUnordered) {
  const double nan = __builtin_nan("");
  EXPECT_NE(static_cast<long long>(isunordered(nan, 1.0)), 0LL);
  EXPECT_NE(static_cast<long long>(isunordered(1.0, nan)), 0LL);
  EXPECT_EQ(static_cast<long long>(isgreater(nan, 1.0)), 0LL);
  EXPECT_EQ(static_cast<long long>(isless(nan, 1.0)), 0LL);
  EXPECT_EQ(static_cast<long long>(isgreaterequal(1.0, nan)), 0LL);
  EXPECT_EQ(static_cast<long long>(islessequal(1.0, nan)), 0LL);
  EXPECT_EQ(static_cast<long long>(islessgreater(nan, 1.0)), 0LL);
}

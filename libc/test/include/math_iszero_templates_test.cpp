//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for iszero and issubnormal being templates in C++
///
//===----------------------------------------------------------------------===//

#include "include/math.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcMathIszeroTemplatesTest, IszeroTakesAnyArithmeticType) {
  EXPECT_TRUE(iszero(0));
  EXPECT_TRUE(iszero(0.0f));
  EXPECT_TRUE(iszero(0.0));
  EXPECT_FALSE(iszero(1));
  EXPECT_FALSE(iszero(1.0));
}

TEST(LlvmLibcMathIszeroTemplatesTest, IssubnormalSeesTheSmallestValues) {
  EXPECT_TRUE(issubnormal(__DBL_DENORM_MIN__));
  EXPECT_FALSE(issubnormal(1.0));
  EXPECT_FALSE(issubnormal(0.0));
}

// A program may declare functions of these names.
namespace user {
bool iszero(int x) { return x == 0; }
} // namespace user

TEST(LlvmLibcMathIszeroTemplatesTest, ANameInAnotherNamespaceIsLeftAlone) {
  EXPECT_TRUE(user::iszero(0));
}

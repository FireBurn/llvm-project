//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the integer constant macros of stdint.h and the word size
/// of limits.h.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/limits-macros.h"
#include "include/llvm-libc-macros/stdint-macros.h"

// The narrow constant macros give a value of the type the integer promotions
// make of the narrow type, which is int, so subtracting from zero is negative.
TEST(LlvmLibcStdintMacrosTest, NarrowConstantsArePromoted) {
  EXPECT_TRUE(UINT8_C(0) - 1 < 0);
  EXPECT_TRUE(UINT16_C(0) - 1 < 0);
  EXPECT_TRUE(UINT8_MAX == 255);
  EXPECT_TRUE(UINT16_MAX == 65535);
}

TEST(LlvmLibcLimitsMacrosTest, WordAndBool) {
  EXPECT_EQ(WORD_BIT, static_cast<int>(sizeof(int) * CHAR_BIT));
  EXPECT_EQ(BOOL_MAX, 1);
}

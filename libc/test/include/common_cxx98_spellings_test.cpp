//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the C keywords the public headers spell for C++
///
//===----------------------------------------------------------------------===//

#include "include/stdlib.h"
#include "test/UnitTest/Test.h"

_Static_assert(sizeof(int) >= 2, "an int has at least two bytes");

TEST(LlvmLibcCommonCxx98SpellingsTest, KeywordSpellingsCompile) {
  _Alignas(16) char buffer[16];
  EXPECT_EQ(reinterpret_cast<unsigned long>(buffer) % 16, 0ul);
  EXPECT_GE(sizeof(buffer), size_t(16));
}

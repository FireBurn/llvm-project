//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for stdbit.h bringing in stdint.h
///
//===----------------------------------------------------------------------===//

#include "include/stdbit.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcStdbitStdintTest, TheFixedWidthTypesAreVisible) {
  uint8_t a = 255;
  uint32_t b = 1;
  uint64_t c = 1;
  EXPECT_EQ(sizeof(a), size_t(1));
  EXPECT_EQ(sizeof(b), size_t(4));
  EXPECT_EQ(sizeof(c), size_t(8));
}

//===-- Unittests for char16_t and char32_t ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for char16_t and char32_t.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-types/char16_t.h"
#include "include/llvm-libc-types/char32_t.h"

TEST(LlvmLibcChar16Char32Test, HaveTheirWidths) {
  ASSERT_EQ(sizeof(char16_t), size_t(2));
  ASSERT_EQ(sizeof(char32_t), size_t(4));
  ASSERT_TRUE(char16_t(-1) > 0);
  ASSERT_TRUE(char32_t(-1) > 0);
}

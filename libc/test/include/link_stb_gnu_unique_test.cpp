//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public link.h header.
///
//===----------------------------------------------------------------------===//

#include "include/link.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcLinkStbGnuUniqueTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"STB_GNU_UNIQUE", static_cast<long long>(STB_GNU_UNIQUE), 10LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}

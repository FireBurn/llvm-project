//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public fcntl.h header.
///
//===----------------------------------------------------------------------===//

#include "include/fcntl.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcFcntlAccessModesTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"F_OK", static_cast<long long>(F_OK), 0LL},
      {"X_OK", static_cast<long long>(X_OK), 1LL},
      {"W_OK", static_cast<long long>(W_OK), 2LL},
      {"R_OK", static_cast<long long>(R_OK), 4LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}

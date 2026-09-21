//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public unistd.h header.
///
//===----------------------------------------------------------------------===//

#include "include/unistd.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcUnistdPosixTimeoutsTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"_POSIX_TIMEOUTS", static_cast<long long>(_POSIX_TIMEOUTS), 0x316a5LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}

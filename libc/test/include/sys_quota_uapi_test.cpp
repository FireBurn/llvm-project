//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public sys/quota.h header.
///
//===----------------------------------------------------------------------===//

#include "include/sys/quota.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysQuotaUapiTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"QIF_BLIMITS", static_cast<long long>(QIF_BLIMITS), 1LL},
      {"QIF_SPACE", static_cast<long long>(QIF_SPACE), 2LL},
      {"QIF_ILIMITS", static_cast<long long>(QIF_ILIMITS), 4LL},
      {"QIF_INODES", static_cast<long long>(QIF_INODES), 8LL},
      {"QIF_BTIME", static_cast<long long>(QIF_BTIME), 16LL},
      {"QIF_ITIME", static_cast<long long>(QIF_ITIME), 32LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}

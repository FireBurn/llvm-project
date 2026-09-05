//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public ar.h header.
///
//===----------------------------------------------------------------------===//

#include "include/ar.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcArTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"SARMAG", static_cast<long long>(SARMAG), 8LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}

TEST(LlvmLibcArTest, StringMacros) {
  EXPECT_STREQ(ARMAG, "!<arch>\n");
  EXPECT_STREQ(ARFMAG, "`\n");
}

TEST(LlvmLibcArTest, TypesAreComplete) {
  EXPECT_GT(sizeof(struct ar_hdr), size_t(0));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(struct ar_hdr), size_t(60));
#endif
}

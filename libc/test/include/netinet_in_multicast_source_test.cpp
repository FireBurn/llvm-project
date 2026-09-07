//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public netinet/in.h header.
///
//===----------------------------------------------------------------------===//

#include "include/netinet/in.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcNetinetInMulticastSourceTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"MCAST_MSFILTER", static_cast<long long>(MCAST_MSFILTER), 48LL},
      {"MCAST_EXCLUDE", static_cast<long long>(MCAST_EXCLUDE), 0LL},
      {"MCAST_INCLUDE", static_cast<long long>(MCAST_INCLUDE), 1LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}

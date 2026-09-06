//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public sys/vt.h header.
///
//===----------------------------------------------------------------------===//

#include "include/sys/vt.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysVtTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"VT_OPENQRY", static_cast<long long>(VT_OPENQRY), 0x5600LL},
      {"VT_GETMODE", static_cast<long long>(VT_GETMODE), 0x5601LL},
      {"VT_SETMODE", static_cast<long long>(VT_SETMODE), 0x5602LL},
      {"VT_GETSTATE", static_cast<long long>(VT_GETSTATE), 0x5603LL},
      {"VT_ACTIVATE", static_cast<long long>(VT_ACTIVATE), 0x5606LL},
      {"VT_WAITACTIVE", static_cast<long long>(VT_WAITACTIVE), 0x5607LL},
      {"VT_DISALLOCATE", static_cast<long long>(VT_DISALLOCATE), 0x5608LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}

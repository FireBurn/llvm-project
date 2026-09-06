//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public sys/kd.h header.
///
//===----------------------------------------------------------------------===//

#include "include/sys/kd.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysKdTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"KDGETMODE", static_cast<long long>(KDGETMODE), 0x4b3bLL},
      {"KDSETMODE", static_cast<long long>(KDSETMODE), 0x4b3aLL},
      {"KDGKBMODE", static_cast<long long>(KDGKBMODE), 0x4b44LL},
      {"KDSKBMODE", static_cast<long long>(KDSKBMODE), 0x4b45LL},
      {"KDGKBTYPE", static_cast<long long>(KDGKBTYPE), 0x4b33LL},
      {"GIO_CMAP", static_cast<long long>(GIO_CMAP), 0x4b70LL},
      {"PIO_CMAP", static_cast<long long>(PIO_CMAP), 0x4b71LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}

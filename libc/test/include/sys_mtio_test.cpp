//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public sys/mtio.h header.
///
//===----------------------------------------------------------------------===//

#include "include/sys/mtio.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysMtioTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"MTIOCGETCONFIG", static_cast<long long>(MTIOCGETCONFIG), 0x80306d04LL},
      {"MTIOCSETCONFIG", static_cast<long long>(MTIOCSETCONFIG), 0x40306d05LL},
      {"MTIOCTOP", static_cast<long long>(MTIOCTOP), 0x40086d01LL},
      {"MTIOCGET", static_cast<long long>(MTIOCGET), 0x80306d02LL},
      {"MTIOCPOS", static_cast<long long>(MTIOCPOS), 0x80086d03LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}

TEST(LlvmLibcSysMtioTest, TypesAreComplete) {
  EXPECT_GT(sizeof(struct mtconfiginfo), size_t(0));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(struct mtconfiginfo), size_t(48));
#endif
}

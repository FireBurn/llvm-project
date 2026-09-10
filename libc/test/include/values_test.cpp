//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public values.h header.
///
//===----------------------------------------------------------------------===//

#include "include/values.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcValuesTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"BITSPERBYTE", static_cast<long long>(BITSPERBYTE), 8LL},
      {"SHORTBITS", static_cast<long long>(SHORTBITS), 16LL},
      {"INTBITS", static_cast<long long>(INTBITS), 32LL},
      {"LONGBITS", static_cast<long long>(LONGBITS), 64LL},
      {"PTRBITS", static_cast<long long>(PTRBITS), 64LL},
      {"DOUBLEBITS", static_cast<long long>(DOUBLEBITS), 64LL},
      {"FLOATBITS", static_cast<long long>(FLOATBITS), 32LL},
      {"MINSHORT", static_cast<long long>(MINSHORT), -0x8000LL},
      {"MININT", static_cast<long long>(MININT), -0x80000000LL},
      {"MINLONG", static_cast<long long>(MINLONG), (-0x7fffffffffffffffLL - 1)},
      {"MAXSHORT", static_cast<long long>(MAXSHORT), 0x7fffLL},
      {"MAXINT", static_cast<long long>(MAXINT), 0x7fffffffLL},
      {"MAXLONG", static_cast<long long>(MAXLONG), 0x7fffffffffffffffLL},
      {"HIBITS", static_cast<long long>(HIBITS), -0x8000LL},
      {"HIBITL", static_cast<long long>(HIBITL), (-0x7fffffffffffffffLL - 1)},
      {"MINDOUBLE", static_cast<long long>(MINDOUBLE), 0LL},
      {"MINFLOAT", static_cast<long long>(MINFLOAT), 0LL},
      {"DMINEXP", static_cast<long long>(DMINEXP), -1021LL},
      {"FMINEXP", static_cast<long long>(FMINEXP), -125LL},
      {"DMAXEXP", static_cast<long long>(DMAXEXP), 1024LL},
      {"FMAXEXP", static_cast<long long>(FMAXEXP), 128LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}

TEST(LlvmLibcValuesTest, MacrosAreDefined) {
#ifndef MAXDOUBLE
  FAIL() << "MAXDOUBLE is not defined";
#endif
#ifndef MAXFLOAT
  FAIL() << "MAXFLOAT is not defined";
#endif
#ifndef BITS
  FAIL() << "BITS is not defined";
#endif
}

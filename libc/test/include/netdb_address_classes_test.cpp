//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public netdb.h header.
///
//===----------------------------------------------------------------------===//

#include "include/netdb.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcNetdbAddressClassesTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"IN_CLASSA_NET", static_cast<long long>(IN_CLASSA_NET), 0xff000000LL},
      {"IN_CLASSA_NSHIFT", static_cast<long long>(IN_CLASSA_NSHIFT), 24LL},
      {"IN_CLASSA_HOST", static_cast<long long>(IN_CLASSA_HOST), 0xffffffLL},
      {"IN_CLASSA_MAX", static_cast<long long>(IN_CLASSA_MAX), 128LL},
      {"IN_CLASSB_NET", static_cast<long long>(IN_CLASSB_NET), 0xffff0000LL},
      {"IN_CLASSB_NSHIFT", static_cast<long long>(IN_CLASSB_NSHIFT), 16LL},
      {"IN_CLASSB_HOST", static_cast<long long>(IN_CLASSB_HOST), 0xffffLL},
      {"IN_CLASSB_MAX", static_cast<long long>(IN_CLASSB_MAX), 0x10000LL},
      {"IN_CLASSC_NET", static_cast<long long>(IN_CLASSC_NET), 0xffffff00LL},
      {"IN_CLASSC_NSHIFT", static_cast<long long>(IN_CLASSC_NSHIFT), 8LL},
      {"IN_CLASSC_HOST", static_cast<long long>(IN_CLASSC_HOST), 255LL},
      {"IN_LOOPBACKNET", static_cast<long long>(IN_LOOPBACKNET), 127LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}

TEST(LlvmLibcNetdbAddressClassesTest, MacrosAreDefined) {
#ifndef IN_CLASSA
  FAIL() << "IN_CLASSA is not defined";
#endif
#ifndef IN_CLASSB
  FAIL() << "IN_CLASSB is not defined";
#endif
#ifndef IN_CLASSC
  FAIL() << "IN_CLASSC is not defined";
#endif
#ifndef IN_CLASSD
  FAIL() << "IN_CLASSD is not defined";
#endif
#ifndef IN_MULTICAST
  FAIL() << "IN_MULTICAST is not defined";
#endif
#ifndef IN_EXPERIMENTAL
  FAIL() << "IN_EXPERIMENTAL is not defined";
#endif
#ifndef IN_BADCLASS
  FAIL() << "IN_BADCLASS is not defined";
#endif
}

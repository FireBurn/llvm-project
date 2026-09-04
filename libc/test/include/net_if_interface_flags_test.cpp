//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public net/if.h header.
///
//===----------------------------------------------------------------------===//

#include "include/net/if.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcNetIfInterfaceFlagsTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"IFF_UP", static_cast<long long>(IFF_UP), 1LL},
      {"IFF_BROADCAST", static_cast<long long>(IFF_BROADCAST), 2LL},
      {"IFF_DEBUG", static_cast<long long>(IFF_DEBUG), 4LL},
      {"IFF_LOOPBACK", static_cast<long long>(IFF_LOOPBACK), 8LL},
      {"IFF_POINTOPOINT", static_cast<long long>(IFF_POINTOPOINT), 16LL},
      {"IFF_NOTRAILERS", static_cast<long long>(IFF_NOTRAILERS), 32LL},
      {"IFF_RUNNING", static_cast<long long>(IFF_RUNNING), 64LL},
      {"IFF_NOARP", static_cast<long long>(IFF_NOARP), 128LL},
      {"IFF_PROMISC", static_cast<long long>(IFF_PROMISC), 256LL},
      {"IFF_ALLMULTI", static_cast<long long>(IFF_ALLMULTI), 512LL},
      {"IFF_MASTER", static_cast<long long>(IFF_MASTER), 1024LL},
      {"IFF_SLAVE", static_cast<long long>(IFF_SLAVE), 2048LL},
      {"IFF_MULTICAST", static_cast<long long>(IFF_MULTICAST), 0x1000LL},
      {"IFF_PORTSEL", static_cast<long long>(IFF_PORTSEL), 0x2000LL},
      {"IFF_AUTOMEDIA", static_cast<long long>(IFF_AUTOMEDIA), 0x4000LL},
      {"IFF_DYNAMIC", static_cast<long long>(IFF_DYNAMIC), 0x8000LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}

TEST(LlvmLibcNetIfInterfaceFlagsTest, TypesAreComplete) {
  EXPECT_GT(sizeof(struct ifconf), size_t(0));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(struct ifconf), size_t(16));
#endif
}

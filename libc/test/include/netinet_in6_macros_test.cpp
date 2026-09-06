//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the IPv6 address macros taking any pointer
///
//===----------------------------------------------------------------------===//

#include "include/netinet/in.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcNetinetIn6MacrosTest, TheyTakeAnyPointer) {
  unsigned char multicast[16] = {0xff, 0x02};
  unsigned char loopback[16] = {};
  loopback[15] = 1;
  unsigned char unspecified[16] = {};
  EXPECT_TRUE(IN6_IS_ADDR_MULTICAST(multicast));
  EXPECT_FALSE(IN6_IS_ADDR_MULTICAST(loopback));
  EXPECT_TRUE(IN6_IS_ADDR_LOOPBACK(loopback));
  EXPECT_TRUE(IN6_IS_ADDR_UNSPECIFIED(unspecified));
  struct in6_addr address;
  __builtin_memcpy(&address, multicast, sizeof(address));
  EXPECT_TRUE(IN6_IS_ADDR_MULTICAST(&address));
}

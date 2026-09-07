//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the parts of an IPv6 address
///
//===----------------------------------------------------------------------===//

#include "include/netinet/in.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcNetinetIn6PartsTest, ThePartsShareTheSixteenBytes) {
  struct in6_addr address;
  __builtin_memset(&address, 0, sizeof(address));
  EXPECT_EQ(static_cast<long long>(sizeof(address)), static_cast<long long>(size_t(16)));
  address.s6_addr[15] = 1;
  EXPECT_EQ(static_cast<long long>(address.s6_addr[15]), static_cast<long long>(1));
  EXPECT_EQ(static_cast<long long>(address.s6_addr32[3] != 0), static_cast<long long>(true));
  address.s6_addr16[0] = 0xabcd;
  EXPECT_EQ(static_cast<long long>(address.s6_addr16[0]), static_cast<long long>(0xabcd));
  EXPECT_EQ(static_cast<long long>(address.s6_addr32[0] & 0xffff), static_cast<long long>(0xabcdu));
}

//===-- Unittests for netinet/ip_icmp.h -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "include/llvm-libc-macros/netinet-ip-icmp-macros.h"
#include "include/llvm-libc-types/struct_icmphdr.h"
#include "test/UnitTest/Test.h"

#include <stddef.h>

// The header goes on the wire as it stands, so its members have to sit where
// the protocol puts them.
TEST(LlvmLibcNetinetIpIcmpTest, HeaderLayout) {
  ASSERT_EQ(sizeof(struct icmphdr), size_t(8));
  ASSERT_EQ(offsetof(struct icmphdr, type), size_t(0));
  ASSERT_EQ(offsetof(struct icmphdr, code), size_t(1));
  ASSERT_EQ(offsetof(struct icmphdr, checksum), size_t(2));
  ASSERT_EQ(offsetof(struct icmphdr, un), size_t(4));
}

// The last four bytes are read differently depending on the kind of message,
// so every reading of them starts at the same place.
TEST(LlvmLibcNetinetIpIcmpTest, TheUnionOverlaysTheSameFourBytes) {
  ASSERT_EQ(offsetof(struct icmphdr, un.echo.id), size_t(4));
  ASSERT_EQ(offsetof(struct icmphdr, un.echo.sequence), size_t(6));
  ASSERT_EQ(offsetof(struct icmphdr, un.gateway), size_t(4));
  ASSERT_EQ(offsetof(struct icmphdr, un.frag.mtu), size_t(6));
}

TEST(LlvmLibcNetinetIpIcmpTest, TheKindsAreNumberedAsTheProtocolSays) {
  ASSERT_EQ(ICMP_ECHOREPLY, 0);
  ASSERT_EQ(ICMP_DEST_UNREACH, 3);
  ASSERT_EQ(ICMP_REDIRECT, 5);
  ASSERT_EQ(ICMP_ECHO, 8);
  ASSERT_EQ(ICMP_TIME_EXCEEDED, 11);
  // Every kind is below the count of them.
  ASSERT_LT(ICMP_TIME_EXCEEDED, NR_ICMP_TYPES);
}

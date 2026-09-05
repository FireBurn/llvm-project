//===-- Unittests for the netinet/in macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the netinet in macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/netinet-in-macros.h"

TEST(LlvmLibcNetinetInMacrosTest, FillInTheMissingSocketAndIpOptionMacros) {
  EXPECT_EQ(static_cast<long long>(IP_TOS), 1LL);
  EXPECT_EQ(static_cast<long long>(IP_TTL), 2LL);
  EXPECT_EQ(static_cast<long long>(IP_HDRINCL), 3LL);
  EXPECT_EQ(static_cast<long long>(IP_OPTIONS), 4LL);
  EXPECT_EQ(static_cast<long long>(IP_ROUTER_ALERT), 5LL);
  EXPECT_EQ(static_cast<long long>(IP_RECVOPTS), 6LL);
  EXPECT_EQ(static_cast<long long>(IP_RETOPTS), 7LL);
  EXPECT_EQ(static_cast<long long>(IP_PKTINFO), 8LL);
  EXPECT_EQ(static_cast<long long>(IP_PKTOPTIONS), 9LL);
  EXPECT_EQ(static_cast<long long>(IP_MTU_DISCOVER), 10LL);
  EXPECT_EQ(static_cast<long long>(IP_RECVERR), 11LL);
  EXPECT_EQ(static_cast<long long>(IP_RECVTTL), 12LL);
  EXPECT_EQ(static_cast<long long>(IP_RECVTOS), 13LL);
  EXPECT_EQ(static_cast<long long>(IP_MTU), 14LL);
  EXPECT_EQ(static_cast<long long>(IP_FREEBIND), 15LL);
  EXPECT_EQ(static_cast<long long>(IP_IPSEC_POLICY), 16LL);
  EXPECT_EQ(static_cast<long long>(IP_XFRM_POLICY), 17LL);
  EXPECT_EQ(static_cast<long long>(IP_PASSSEC), 18LL);
  EXPECT_EQ(static_cast<long long>(IP_TRANSPARENT), 19LL);
  EXPECT_EQ(static_cast<long long>(IP_ORIGDSTADDR), 20LL);
  EXPECT_EQ(static_cast<long long>(IP_RECVORIGDSTADDR), 20LL);
  EXPECT_EQ(static_cast<long long>(IP_MINTTL), 21LL);
  EXPECT_EQ(static_cast<long long>(IP_NODEFRAG), 22LL);
  EXPECT_EQ(static_cast<long long>(IP_CHECKSUM), 23LL);
  EXPECT_EQ(static_cast<long long>(IP_BIND_ADDRESS_NO_PORT), 24LL);
  EXPECT_EQ(static_cast<long long>(IP_RECVFRAGSIZE), 25LL);
  EXPECT_EQ(static_cast<long long>(IP_MULTICAST_IF), 32LL);
  EXPECT_EQ(static_cast<long long>(IP_MULTICAST_TTL), 33LL);
  EXPECT_EQ(static_cast<long long>(IP_MULTICAST_LOOP), 34LL);
  EXPECT_EQ(static_cast<long long>(IP_ADD_MEMBERSHIP), 35LL);
  EXPECT_EQ(static_cast<long long>(IP_DROP_MEMBERSHIP), 36LL);
  EXPECT_EQ(static_cast<long long>(IP_UNBLOCK_SOURCE), 37LL);
  EXPECT_EQ(static_cast<long long>(IP_BLOCK_SOURCE), 38LL);
  EXPECT_EQ(static_cast<long long>(IP_ADD_SOURCE_MEMBERSHIP), 39LL);
  EXPECT_EQ(static_cast<long long>(IP_DROP_SOURCE_MEMBERSHIP), 40LL);
  EXPECT_EQ(static_cast<long long>(IP_MSFILTER), 41LL);
  EXPECT_EQ(static_cast<long long>(IP_MULTICAST_ALL), 49LL);
  EXPECT_EQ(static_cast<long long>(IP_PMTUDISC_DONT), 0LL);
  EXPECT_EQ(static_cast<long long>(IP_PMTUDISC_WANT), 1LL);
  EXPECT_EQ(static_cast<long long>(IP_PMTUDISC_DO), 2LL);
  EXPECT_EQ(static_cast<long long>(IP_PMTUDISC_PROBE), 3LL);
  EXPECT_EQ(static_cast<long long>(IP_PMTUDISC_INTERFACE), 4LL);
  EXPECT_EQ(static_cast<long long>(IP_PMTUDISC_OMIT), 5LL);
  EXPECT_EQ(static_cast<long long>(IPV6_ADDRFORM), 1LL);
  EXPECT_EQ(static_cast<long long>(IPV6_2292PKTINFO), 2LL);
  EXPECT_EQ(static_cast<long long>(IPV6_2292HOPOPTS), 3LL);
  EXPECT_EQ(static_cast<long long>(IPV6_2292DSTOPTS), 4LL);
  EXPECT_EQ(static_cast<long long>(IPV6_2292RTHDR), 5LL);
  EXPECT_EQ(static_cast<long long>(IPV6_2292PKTOPTIONS), 6LL);
  EXPECT_EQ(static_cast<long long>(IPV6_CHECKSUM), 7LL);
  EXPECT_EQ(static_cast<long long>(IPV6_2292HOPLIMIT), 8LL);
  EXPECT_EQ(static_cast<long long>(IPV6_NEXTHOP), 9LL);
  EXPECT_EQ(static_cast<long long>(IPV6_FLOWINFO), 11LL);
  EXPECT_EQ(static_cast<long long>(IPV6_UNICAST_HOPS), 16LL);
  EXPECT_EQ(static_cast<long long>(IPV6_MULTICAST_IF), 17LL);
  EXPECT_EQ(static_cast<long long>(IPV6_MULTICAST_HOPS), 18LL);
  EXPECT_EQ(static_cast<long long>(IPV6_MULTICAST_LOOP), 19LL);
  EXPECT_EQ(static_cast<long long>(IPV6_ADD_MEMBERSHIP), 20LL);
  EXPECT_EQ(static_cast<long long>(IPV6_DROP_MEMBERSHIP), 21LL);
  EXPECT_EQ(static_cast<long long>(IPV6_JOIN_GROUP), 20LL);
  EXPECT_EQ(static_cast<long long>(IPV6_LEAVE_GROUP), 21LL);
  EXPECT_EQ(static_cast<long long>(IPV6_ROUTER_ALERT), 22LL);
  EXPECT_EQ(static_cast<long long>(IPV6_MTU_DISCOVER), 23LL);
  EXPECT_EQ(static_cast<long long>(IPV6_MTU), 24LL);
  EXPECT_EQ(static_cast<long long>(IPV6_RECVERR), 25LL);
  EXPECT_EQ(static_cast<long long>(IPV6_V6ONLY), 26LL);
  EXPECT_EQ(static_cast<long long>(IPV6_JOIN_ANYCAST), 27LL);
  EXPECT_EQ(static_cast<long long>(IPV6_LEAVE_ANYCAST), 28LL);
  EXPECT_EQ(static_cast<long long>(IPV6_MULTICAST_ALL), 29LL);
  EXPECT_EQ(static_cast<long long>(IPV6_ROUTER_ALERT_ISOLATE), 30LL);
  EXPECT_EQ(static_cast<long long>(IPV6_RECVERR_RFC4884), 31LL);
  EXPECT_EQ(static_cast<long long>(IPV6_RECVPKTINFO), 49LL);
  EXPECT_EQ(static_cast<long long>(IPV6_PKTINFO), 50LL);
  EXPECT_EQ(static_cast<long long>(IPV6_RECVHOPLIMIT), 51LL);
  EXPECT_EQ(static_cast<long long>(IPV6_HOPLIMIT), 52LL);
  EXPECT_EQ(static_cast<long long>(IPV6_RECVHOPOPTS), 53LL);
  EXPECT_EQ(static_cast<long long>(IPV6_HOPOPTS), 54LL);
  EXPECT_EQ(static_cast<long long>(IPV6_RTHDRDSTOPTS), 55LL);
  EXPECT_EQ(static_cast<long long>(IPV6_RECVRTHDR), 56LL);
  EXPECT_EQ(static_cast<long long>(IPV6_RTHDR), 57LL);
  EXPECT_EQ(static_cast<long long>(IPV6_RECVDSTOPTS), 58LL);
  EXPECT_EQ(static_cast<long long>(IPV6_DSTOPTS), 59LL);
  EXPECT_EQ(static_cast<long long>(IPV6_RECVPATHMTU), 60LL);
  EXPECT_EQ(static_cast<long long>(IPV6_PATHMTU), 61LL);
  EXPECT_EQ(static_cast<long long>(IPV6_DONTFRAG), 62LL);
  EXPECT_EQ(static_cast<long long>(IPV6_RECVTCLASS), 66LL);
  EXPECT_EQ(static_cast<long long>(IPV6_TCLASS), 67LL);
  EXPECT_EQ(static_cast<long long>(IPV6_AUTOFLOWLABEL), 70LL);
  EXPECT_EQ(static_cast<long long>(IPV6_ADDR_PREFERENCES), 72LL);
  EXPECT_EQ(static_cast<long long>(IPV6_MINHOPCOUNT), 73LL);
  EXPECT_EQ(static_cast<long long>(IPV6_ORIGDSTADDR), 74LL);
  EXPECT_EQ(static_cast<long long>(IPV6_RECVORIGDSTADDR), 74LL);
  EXPECT_EQ(static_cast<long long>(IPV6_TRANSPARENT), 75LL);
  EXPECT_EQ(static_cast<long long>(IPV6_UNICAST_IF), 76LL);
  EXPECT_EQ(static_cast<long long>(IPV6_RECVFRAGSIZE), 77LL);
  EXPECT_EQ(static_cast<long long>(IPV6_FREEBIND), 78LL);
  EXPECT_EQ(static_cast<long long>(IPV6_PMTUDISC_DONT), 0LL);
  EXPECT_EQ(static_cast<long long>(IPV6_PMTUDISC_WANT), 1LL);
  EXPECT_EQ(static_cast<long long>(IPV6_PMTUDISC_DO), 2LL);
  EXPECT_EQ(static_cast<long long>(IPV6_PMTUDISC_PROBE), 3LL);
  EXPECT_EQ(static_cast<long long>(IPV6_PMTUDISC_INTERFACE), 4LL);
  EXPECT_EQ(static_cast<long long>(IPV6_PMTUDISC_OMIT), 5LL);
}

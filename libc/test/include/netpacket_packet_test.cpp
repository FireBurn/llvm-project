//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public netpacket/packet.h header.
///
//===----------------------------------------------------------------------===//

#include "include/netpacket/packet.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcNetpacketPacketTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"PACKET_HOST", static_cast<long long>(PACKET_HOST), 0LL},
      {"PACKET_BROADCAST", static_cast<long long>(PACKET_BROADCAST), 1LL},
      {"PACKET_MULTICAST", static_cast<long long>(PACKET_MULTICAST), 2LL},
      {"PACKET_OTHERHOST", static_cast<long long>(PACKET_OTHERHOST), 3LL},
      {"PACKET_OUTGOING", static_cast<long long>(PACKET_OUTGOING), 4LL},
      {"PACKET_LOOPBACK", static_cast<long long>(PACKET_LOOPBACK), 5LL},
      {"PACKET_USER", static_cast<long long>(PACKET_USER), 6LL},
      {"PACKET_KERNEL", static_cast<long long>(PACKET_KERNEL), 7LL},
      {"PACKET_FASTROUTE", static_cast<long long>(PACKET_FASTROUTE), 6LL},
      {"PACKET_ADD_MEMBERSHIP", static_cast<long long>(PACKET_ADD_MEMBERSHIP), 1LL},
      {"PACKET_DROP_MEMBERSHIP", static_cast<long long>(PACKET_DROP_MEMBERSHIP), 2LL},
      {"PACKET_RECV_OUTPUT", static_cast<long long>(PACKET_RECV_OUTPUT), 3LL},
      {"PACKET_RX_RING", static_cast<long long>(PACKET_RX_RING), 5LL},
      {"PACKET_STATISTICS", static_cast<long long>(PACKET_STATISTICS), 6LL},
      {"PACKET_COPY_THRESH", static_cast<long long>(PACKET_COPY_THRESH), 7LL},
      {"PACKET_AUXDATA", static_cast<long long>(PACKET_AUXDATA), 8LL},
      {"PACKET_ORIGDEV", static_cast<long long>(PACKET_ORIGDEV), 9LL},
      {"PACKET_VERSION", static_cast<long long>(PACKET_VERSION), 10LL},
      {"PACKET_HDRLEN", static_cast<long long>(PACKET_HDRLEN), 11LL},
      {"PACKET_RESERVE", static_cast<long long>(PACKET_RESERVE), 12LL},
      {"PACKET_TX_RING", static_cast<long long>(PACKET_TX_RING), 13LL},
      {"PACKET_LOSS", static_cast<long long>(PACKET_LOSS), 14LL},
      {"PACKET_VNET_HDR", static_cast<long long>(PACKET_VNET_HDR), 15LL},
      {"PACKET_TX_TIMESTAMP", static_cast<long long>(PACKET_TX_TIMESTAMP), 16LL},
      {"PACKET_TIMESTAMP", static_cast<long long>(PACKET_TIMESTAMP), 17LL},
      {"PACKET_FANOUT", static_cast<long long>(PACKET_FANOUT), 18LL},
      {"PACKET_QDISC_BYPASS", static_cast<long long>(PACKET_QDISC_BYPASS), 20LL},
      {"PACKET_MR_MULTICAST", static_cast<long long>(PACKET_MR_MULTICAST), 0LL},
      {"PACKET_MR_PROMISC", static_cast<long long>(PACKET_MR_PROMISC), 1LL},
      {"PACKET_MR_ALLMULTI", static_cast<long long>(PACKET_MR_ALLMULTI), 2LL},
      {"PACKET_MR_UNICAST", static_cast<long long>(PACKET_MR_UNICAST), 3LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}

TEST(LlvmLibcNetpacketPacketTest, TypesAreComplete) {
  EXPECT_GT(sizeof(struct sockaddr_ll), size_t(0));
  EXPECT_GT(sizeof(struct packet_mreq), size_t(0));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(struct sockaddr_ll), size_t(20));
  EXPECT_EQ(sizeof(struct packet_mreq), size_t(16));
#endif
}

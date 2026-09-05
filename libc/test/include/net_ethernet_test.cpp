//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public net/ethernet.h header.
///
//===----------------------------------------------------------------------===//

#include "include/net/ethernet.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcNetEthernetTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"ETH_ALEN", static_cast<long long>(ETH_ALEN), 6LL},
      {"ETH_TLEN", static_cast<long long>(ETH_TLEN), 2LL},
      {"ETH_HLEN", static_cast<long long>(ETH_HLEN), 14LL},
      {"ETH_ZLEN", static_cast<long long>(ETH_ZLEN), 60LL},
      {"ETH_DATA_LEN", static_cast<long long>(ETH_DATA_LEN), 1500LL},
      {"ETH_FRAME_LEN", static_cast<long long>(ETH_FRAME_LEN), 1514LL},
      {"ETH_FCS_LEN", static_cast<long long>(ETH_FCS_LEN), 4LL},
      {"ETH_MIN_MTU", static_cast<long long>(ETH_MIN_MTU), 68LL},
      {"ETH_MAX_MTU", static_cast<long long>(ETH_MAX_MTU), 0xffffLL},
      {"ETHER_ADDR_LEN", static_cast<long long>(ETHER_ADDR_LEN), 6LL},
      {"ETHER_TYPE_LEN", static_cast<long long>(ETHER_TYPE_LEN), 2LL},
      {"ETHER_CRC_LEN", static_cast<long long>(ETHER_CRC_LEN), 4LL},
      {"ETHER_HDR_LEN", static_cast<long long>(ETHER_HDR_LEN), 14LL},
      {"ETHER_MIN_LEN", static_cast<long long>(ETHER_MIN_LEN), 64LL},
      {"ETHER_MAX_LEN", static_cast<long long>(ETHER_MAX_LEN), 1518LL},
      {"ETHERTYPE_PUP", static_cast<long long>(ETHERTYPE_PUP), 512LL},
      {"ETHERTYPE_SPRITE", static_cast<long long>(ETHERTYPE_SPRITE), 1280LL},
      {"ETHERTYPE_IP", static_cast<long long>(ETHERTYPE_IP), 2048LL},
      {"ETHERTYPE_ARP", static_cast<long long>(ETHERTYPE_ARP), 2054LL},
      {"ETHERTYPE_REVARP", static_cast<long long>(ETHERTYPE_REVARP), 0x8035LL},
      {"ETHERTYPE_AT", static_cast<long long>(ETHERTYPE_AT), 0x809bLL},
      {"ETHERTYPE_AARP", static_cast<long long>(ETHERTYPE_AARP), 0x80f3LL},
      {"ETHERTYPE_VLAN", static_cast<long long>(ETHERTYPE_VLAN), 0x8100LL},
      {"ETHERTYPE_IPX", static_cast<long long>(ETHERTYPE_IPX), 0x8137LL},
      {"ETHERTYPE_IPV6", static_cast<long long>(ETHERTYPE_IPV6), 0x86ddLL},
      {"ETHERTYPE_LOOPBACK", static_cast<long long>(ETHERTYPE_LOOPBACK), 0x9000LL},
      {"ETHERTYPE_TRAIL", static_cast<long long>(ETHERTYPE_TRAIL), 0x1000LL},
      {"ETHERTYPE_NTRAILER", static_cast<long long>(ETHERTYPE_NTRAILER), 16LL},
      {"ETH_P_LOOP", static_cast<long long>(ETH_P_LOOP), 96LL},
      {"ETH_P_PUP", static_cast<long long>(ETH_P_PUP), 512LL},
      {"ETH_P_PUPAT", static_cast<long long>(ETH_P_PUPAT), 513LL},
      {"ETH_P_TSN", static_cast<long long>(ETH_P_TSN), 0x22f0LL},
      {"ETH_P_ERSPAN2", static_cast<long long>(ETH_P_ERSPAN2), 0x22ebLL},
      {"ETH_P_IP", static_cast<long long>(ETH_P_IP), 2048LL},
      {"ETH_P_X25", static_cast<long long>(ETH_P_X25), 2053LL},
      {"ETH_P_ARP", static_cast<long long>(ETH_P_ARP), 2054LL},
      {"ETH_P_BPQ", static_cast<long long>(ETH_P_BPQ), 2303LL},
      {"ETH_P_IEEEPUP", static_cast<long long>(ETH_P_IEEEPUP), 2560LL},
      {"ETH_P_IEEEPUPAT", static_cast<long long>(ETH_P_IEEEPUPAT), 2561LL},
      {"ETH_P_BATMAN", static_cast<long long>(ETH_P_BATMAN), 0x4305LL},
      {"ETH_P_DEC", static_cast<long long>(ETH_P_DEC), 0x6000LL},
      {"ETH_P_DNA_DL", static_cast<long long>(ETH_P_DNA_DL), 0x6001LL},
      {"ETH_P_DNA_RC", static_cast<long long>(ETH_P_DNA_RC), 0x6002LL},
      {"ETH_P_DNA_RT", static_cast<long long>(ETH_P_DNA_RT), 0x6003LL},
      {"ETH_P_LAT", static_cast<long long>(ETH_P_LAT), 0x6004LL},
      {"ETH_P_DIAG", static_cast<long long>(ETH_P_DIAG), 0x6005LL},
      {"ETH_P_CUST", static_cast<long long>(ETH_P_CUST), 0x6006LL},
      {"ETH_P_SCA", static_cast<long long>(ETH_P_SCA), 0x6007LL},
      {"ETH_P_TEB", static_cast<long long>(ETH_P_TEB), 0x6558LL},
      {"ETH_P_RARP", static_cast<long long>(ETH_P_RARP), 0x8035LL},
      {"ETH_P_ATALK", static_cast<long long>(ETH_P_ATALK), 0x809bLL},
      {"ETH_P_AARP", static_cast<long long>(ETH_P_AARP), 0x80f3LL},
      {"ETH_P_8021Q", static_cast<long long>(ETH_P_8021Q), 0x8100LL},
      {"ETH_P_ERSPAN", static_cast<long long>(ETH_P_ERSPAN), 0x88beLL},
      {"ETH_P_IPX", static_cast<long long>(ETH_P_IPX), 0x8137LL},
      {"ETH_P_IPV6", static_cast<long long>(ETH_P_IPV6), 0x86ddLL},
      {"ETH_P_PAUSE", static_cast<long long>(ETH_P_PAUSE), 0x8808LL},
      {"ETH_P_SLOW", static_cast<long long>(ETH_P_SLOW), 0x8809LL},
      {"ETH_P_WCCP", static_cast<long long>(ETH_P_WCCP), 0x883eLL},
      {"ETH_P_MPLS_UC", static_cast<long long>(ETH_P_MPLS_UC), 0x8847LL},
      {"ETH_P_MPLS_MC", static_cast<long long>(ETH_P_MPLS_MC), 0x8848LL},
      {"ETH_P_ATMMPOA", static_cast<long long>(ETH_P_ATMMPOA), 0x884cLL},
      {"ETH_P_PPP_DISC", static_cast<long long>(ETH_P_PPP_DISC), 0x8863LL},
      {"ETH_P_PPP_SES", static_cast<long long>(ETH_P_PPP_SES), 0x8864LL},
      {"ETH_P_LINK_CTL", static_cast<long long>(ETH_P_LINK_CTL), 0x886cLL},
      {"ETH_P_ATMFATE", static_cast<long long>(ETH_P_ATMFATE), 0x8884LL},
      {"ETH_P_PAE", static_cast<long long>(ETH_P_PAE), 0x888eLL},
      {"ETH_P_PROFINET", static_cast<long long>(ETH_P_PROFINET), 0x8892LL},
      {"ETH_P_REALTEK", static_cast<long long>(ETH_P_REALTEK), 0x8899LL},
      {"ETH_P_AOE", static_cast<long long>(ETH_P_AOE), 0x88a2LL},
      {"ETH_P_ETHERCAT", static_cast<long long>(ETH_P_ETHERCAT), 0x88a4LL},
      {"ETH_P_8021AD", static_cast<long long>(ETH_P_8021AD), 0x88a8LL},
      {"ETH_P_802_EX1", static_cast<long long>(ETH_P_802_EX1), 0x88b5LL},
      {"ETH_P_MXLGSW", static_cast<long long>(ETH_P_MXLGSW), 0x88c3LL},
      {"ETH_P_PREAUTH", static_cast<long long>(ETH_P_PREAUTH), 0x88c7LL},
      {"ETH_P_TIPC", static_cast<long long>(ETH_P_TIPC), 0x88caLL},
      {"ETH_P_LLDP", static_cast<long long>(ETH_P_LLDP), 0x88ccLL},
      {"ETH_P_MRP", static_cast<long long>(ETH_P_MRP), 0x88e3LL},
      {"ETH_P_MACSEC", static_cast<long long>(ETH_P_MACSEC), 0x88e5LL},
      {"ETH_P_8021AH", static_cast<long long>(ETH_P_8021AH), 0x88e7LL},
      {"ETH_P_MVRP", static_cast<long long>(ETH_P_MVRP), 0x88f5LL},
      {"ETH_P_1588", static_cast<long long>(ETH_P_1588), 0x88f7LL},
      {"ETH_P_NCSI", static_cast<long long>(ETH_P_NCSI), 0x88f8LL},
      {"ETH_P_PRP", static_cast<long long>(ETH_P_PRP), 0x88fbLL},
      {"ETH_P_CFM", static_cast<long long>(ETH_P_CFM), 0x8902LL},
      {"ETH_P_FCOE", static_cast<long long>(ETH_P_FCOE), 0x8906LL},
      {"ETH_P_IBOE", static_cast<long long>(ETH_P_IBOE), 0x8915LL},
      {"ETH_P_TDLS", static_cast<long long>(ETH_P_TDLS), 0x890dLL},
      {"ETH_P_FIP", static_cast<long long>(ETH_P_FIP), 0x8914LL},
      {"ETH_P_80221", static_cast<long long>(ETH_P_80221), 0x8917LL},
      {"ETH_P_HSR", static_cast<long long>(ETH_P_HSR), 0x892fLL},
      {"ETH_P_NSH", static_cast<long long>(ETH_P_NSH), 0x894fLL},
      {"ETH_P_LOOPBACK", static_cast<long long>(ETH_P_LOOPBACK), 0x9000LL},
      {"ETH_P_QINQ1", static_cast<long long>(ETH_P_QINQ1), 0x9100LL},
      {"ETH_P_QINQ2", static_cast<long long>(ETH_P_QINQ2), 0x9200LL},
      {"ETH_P_QINQ3", static_cast<long long>(ETH_P_QINQ3), 0x9300LL},
      {"ETH_P_YT921X", static_cast<long long>(ETH_P_YT921X), 0x9988LL},
      {"ETH_P_EDSA", static_cast<long long>(ETH_P_EDSA), 0xdadaLL},
      {"ETH_P_DSA_8021Q", static_cast<long long>(ETH_P_DSA_8021Q), 0xdadbLL},
      {"ETH_P_DSA_A5PSW", static_cast<long long>(ETH_P_DSA_A5PSW), 0xe001LL},
      {"ETH_P_IFE", static_cast<long long>(ETH_P_IFE), 0xed3eLL},
      {"ETH_P_AF_IUCV", static_cast<long long>(ETH_P_AF_IUCV), 0xfbfbLL},
      {"ETH_P_802_3_MIN", static_cast<long long>(ETH_P_802_3_MIN), 1536LL},
      {"ETH_P_802_3", static_cast<long long>(ETH_P_802_3), 1LL},
      {"ETH_P_AX25", static_cast<long long>(ETH_P_AX25), 2LL},
      {"ETH_P_ALL", static_cast<long long>(ETH_P_ALL), 3LL},
      {"ETH_P_802_2", static_cast<long long>(ETH_P_802_2), 4LL},
      {"ETH_P_SNAP", static_cast<long long>(ETH_P_SNAP), 5LL},
      {"ETH_P_DDCMP", static_cast<long long>(ETH_P_DDCMP), 6LL},
      {"ETH_P_WAN_PPP", static_cast<long long>(ETH_P_WAN_PPP), 7LL},
      {"ETH_P_PPP_MP", static_cast<long long>(ETH_P_PPP_MP), 8LL},
      {"ETH_P_LOCALTALK", static_cast<long long>(ETH_P_LOCALTALK), 9LL},
      {"ETH_P_CAN", static_cast<long long>(ETH_P_CAN), 12LL},
      {"ETH_P_CANFD", static_cast<long long>(ETH_P_CANFD), 13LL},
      {"ETH_P_CANXL", static_cast<long long>(ETH_P_CANXL), 14LL},
      {"ETH_P_PPPTALK", static_cast<long long>(ETH_P_PPPTALK), 16LL},
      {"ETH_P_TR_802_2", static_cast<long long>(ETH_P_TR_802_2), 17LL},
      {"ETH_P_MOBITEX", static_cast<long long>(ETH_P_MOBITEX), 21LL},
      {"ETH_P_CONTROL", static_cast<long long>(ETH_P_CONTROL), 22LL},
      {"ETH_P_IRDA", static_cast<long long>(ETH_P_IRDA), 23LL},
      {"ETH_P_ECONET", static_cast<long long>(ETH_P_ECONET), 24LL},
      {"ETH_P_HDLC", static_cast<long long>(ETH_P_HDLC), 25LL},
      {"ETH_P_ARCNET", static_cast<long long>(ETH_P_ARCNET), 26LL},
      {"ETH_P_DSA", static_cast<long long>(ETH_P_DSA), 27LL},
      {"ETH_P_TRAILER", static_cast<long long>(ETH_P_TRAILER), 28LL},
      {"ETH_P_PHONET", static_cast<long long>(ETH_P_PHONET), 245LL},
      {"ETH_P_IEEE802154", static_cast<long long>(ETH_P_IEEE802154), 246LL},
      {"ETH_P_CAIF", static_cast<long long>(ETH_P_CAIF), 247LL},
      {"ETH_P_XDSA", static_cast<long long>(ETH_P_XDSA), 248LL},
      {"ETH_P_MAP", static_cast<long long>(ETH_P_MAP), 249LL},
      {"ETH_P_MCTP", static_cast<long long>(ETH_P_MCTP), 250LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}

TEST(LlvmLibcNetEthernetTest, MacrosAreDefined) {
#ifndef ETHER_IS_VALID_LEN
  FAIL() << "ETHER_IS_VALID_LEN is not defined";
#endif
}

TEST(LlvmLibcNetEthernetTest, TypesAreComplete) {
  EXPECT_GT(sizeof(struct ether_header), size_t(0));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(struct ether_header), size_t(14));
#endif
}

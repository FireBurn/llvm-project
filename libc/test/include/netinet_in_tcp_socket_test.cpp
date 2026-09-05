//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public netinet/in.h header.
///
//===----------------------------------------------------------------------===//

#include "include/netinet/in.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcNetinetInTcpSocketTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"AF_MPLS", static_cast<long long>(AF_MPLS), 28LL},
      {"AF_CAN", static_cast<long long>(AF_CAN), 29LL},
      {"AF_ALG", static_cast<long long>(AF_ALG), 38LL},
      {"AF_XDP", static_cast<long long>(AF_XDP), 44LL},
      {"PF_MPLS", static_cast<long long>(PF_MPLS), 28LL},
      {"PF_CAN", static_cast<long long>(PF_CAN), 29LL},
      {"PF_ALG", static_cast<long long>(PF_ALG), 38LL},
      {"PF_XDP", static_cast<long long>(PF_XDP), 44LL},
      {"SCM_PIDFD", static_cast<long long>(SCM_PIDFD), 4LL},
      {"IPPROTO_IGMP", static_cast<long long>(IPPROTO_IGMP), 2LL},
      {"IPPROTO_IPIP", static_cast<long long>(IPPROTO_IPIP), 4LL},
      {"IPPROTO_EGP", static_cast<long long>(IPPROTO_EGP), 8LL},
      {"IPPROTO_PUP", static_cast<long long>(IPPROTO_PUP), 12LL},
      {"IPPROTO_IDP", static_cast<long long>(IPPROTO_IDP), 22LL},
      {"IPPROTO_TP", static_cast<long long>(IPPROTO_TP), 29LL},
      {"IPPROTO_DCCP", static_cast<long long>(IPPROTO_DCCP), 33LL},
      {"IPPROTO_RSVP", static_cast<long long>(IPPROTO_RSVP), 46LL},
      {"IPPROTO_GRE", static_cast<long long>(IPPROTO_GRE), 47LL},
      {"IPPROTO_ESP", static_cast<long long>(IPPROTO_ESP), 50LL},
      {"IPPROTO_AH", static_cast<long long>(IPPROTO_AH), 51LL},
      {"IPPROTO_MTP", static_cast<long long>(IPPROTO_MTP), 92LL},
      {"IPPROTO_BEETPH", static_cast<long long>(IPPROTO_BEETPH), 94LL},
      {"IPPROTO_ENCAP", static_cast<long long>(IPPROTO_ENCAP), 98LL},
      {"IPPROTO_PIM", static_cast<long long>(IPPROTO_PIM), 103LL},
      {"IPPROTO_COMP", static_cast<long long>(IPPROTO_COMP), 108LL},
      {"IPPROTO_L2TP", static_cast<long long>(IPPROTO_L2TP), 115LL},
      {"IPPROTO_SCTP", static_cast<long long>(IPPROTO_SCTP), 132LL},
      {"IPPROTO_UDPLITE", static_cast<long long>(IPPROTO_UDPLITE), 136LL},
      {"IPPROTO_MPLS", static_cast<long long>(IPPROTO_MPLS), 137LL},
      {"IPPROTO_ETHERNET", static_cast<long long>(IPPROTO_ETHERNET), 143LL},
      {"IPPROTO_AGGFRAG", static_cast<long long>(IPPROTO_AGGFRAG), 144LL},
      {"IPPROTO_SMC", static_cast<long long>(IPPROTO_SMC), 256LL},
      {"IPPROTO_MPTCP", static_cast<long long>(IPPROTO_MPTCP), 262LL},
      {"IPPROTO_HOPOPTS", static_cast<long long>(IPPROTO_HOPOPTS), 0LL},
      {"IPPROTO_ROUTING", static_cast<long long>(IPPROTO_ROUTING), 43LL},
      {"IPPROTO_FRAGMENT", static_cast<long long>(IPPROTO_FRAGMENT), 44LL},
      {"IPPROTO_ICMPV6", static_cast<long long>(IPPROTO_ICMPV6), 58LL},
      {"IPPROTO_NONE", static_cast<long long>(IPPROTO_NONE), 59LL},
      {"IPPROTO_DSTOPTS", static_cast<long long>(IPPROTO_DSTOPTS), 60LL},
      {"IPPROTO_MH", static_cast<long long>(IPPROTO_MH), 135LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}

TEST(LlvmLibcNetinetInTcpSocketTest, TypesAreComplete) {
  EXPECT_GT(sizeof(struct in_pktinfo), size_t(0));
  EXPECT_GT(sizeof(struct in6_pktinfo), size_t(0));
  EXPECT_GT(sizeof(struct ip6_mtuinfo), size_t(0));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(struct in_pktinfo), size_t(12));
  EXPECT_EQ(sizeof(struct in6_pktinfo), size_t(20));
  EXPECT_EQ(sizeof(struct ip6_mtuinfo), size_t(32));
#endif
}

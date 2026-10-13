//===-- Unittests for the linux/sys/socket macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the linux sys socket macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/sys-socket-macros.h"

TEST(LlvmLibcLinuxSysSocketMacrosTest,
     FillInTheMissingSocketAndIpOptionMacros) {
  EXPECT_EQ(static_cast<long long>(AF_BRIDGE), 7LL);
  EXPECT_EQ(static_cast<long long>(AF_PACKET), 17LL);
  EXPECT_EQ(static_cast<long long>(AF_VSOCK), 40LL);
  EXPECT_EQ(static_cast<long long>(PF_BRIDGE), 7LL);
  EXPECT_EQ(static_cast<long long>(PF_PACKET), 17LL);
  EXPECT_EQ(static_cast<long long>(PF_VSOCK), 40LL);
  EXPECT_EQ(static_cast<long long>(SOL_RAW), 255LL);
  EXPECT_EQ(static_cast<long long>(SOL_PACKET), 263LL);
  EXPECT_EQ(static_cast<long long>(SOL_NETLINK), 270LL);
  EXPECT_EQ(static_cast<long long>(SCM_CREDENTIALS), 2LL);
  EXPECT_EQ(static_cast<long long>(SCM_SECURITY), 3LL);
}

TEST(LlvmLibcLinuxSysSocketMacrosTest, AddScm_TimestampnsAndScm_Timestamping) {
  EXPECT_EQ(static_cast<long long>(SCM_TIMESTAMP), 29LL);
  EXPECT_EQ(static_cast<long long>(SCM_TIMESTAMPNS), 35LL);
  EXPECT_EQ(static_cast<long long>(SCM_TIMESTAMPING), 37LL);
}

TEST(LlvmLibcLinuxSysSocketMacrosTest, AddSomaxconn) {
  EXPECT_EQ(static_cast<long long>(SOMAXCONN), 0x1000LL);
  EXPECT_EQ(static_cast<long long>(SOL_IP), 0LL);
  EXPECT_EQ(static_cast<long long>(SOL_TCP), 6LL);
  EXPECT_EQ(static_cast<long long>(SOL_UDP), 17LL);
  EXPECT_EQ(static_cast<long long>(SOL_IPV6), 41LL);
  EXPECT_EQ(static_cast<long long>(SOL_ICMPV6), 58LL);
  EXPECT_EQ(static_cast<long long>(SOL_SCTP), 132LL);
  EXPECT_EQ(static_cast<long long>(SOL_UDPLITE), 136LL);
}

TEST(LlvmLibcLinuxSysSocketMacrosTest, AddSock_Dccp) {
  EXPECT_EQ(static_cast<long long>(SOCK_DCCP), 6LL);
}

TEST(LlvmLibcLinuxSysSocketMacrosTest,
     NameTheControlMessagesForWifiStatusAndTr) {
  EXPECT_EQ(static_cast<long long>(SCM_WIFI_STATUS), 41LL);
  EXPECT_EQ(static_cast<long long>(SCM_TXTIME), 61LL);
}

TEST(LlvmLibcLinuxSysSocketMacrosTest, AddTheRemainingSetsockoptLevels) {
  EXPECT_EQ(static_cast<long long>(SOL_DECNET), 261LL);
  EXPECT_EQ(static_cast<long long>(SOL_X25), 262LL);
  EXPECT_EQ(static_cast<long long>(SOL_ATM), 264LL);
  EXPECT_EQ(static_cast<long long>(SOL_AAL), 265LL);
  EXPECT_EQ(static_cast<long long>(SOL_IRDA), 266LL);
  EXPECT_EQ(static_cast<long long>(SOL_NETBEUI), 267LL);
  EXPECT_EQ(static_cast<long long>(SOL_LLC), 268LL);
  EXPECT_EQ(static_cast<long long>(SOL_DCCP), 269LL);
  EXPECT_EQ(static_cast<long long>(SOL_TIPC), 271LL);
  EXPECT_EQ(static_cast<long long>(SOL_RXRPC), 272LL);
  EXPECT_EQ(static_cast<long long>(SOL_PPPOL2TP), 273LL);
  EXPECT_EQ(static_cast<long long>(SOL_BLUETOOTH), 274LL);
  EXPECT_EQ(static_cast<long long>(SOL_PNPIPE), 275LL);
  EXPECT_EQ(static_cast<long long>(SOL_RDS), 276LL);
  EXPECT_EQ(static_cast<long long>(SOL_IUCV), 277LL);
  EXPECT_EQ(static_cast<long long>(SOL_CAIF), 278LL);
  EXPECT_EQ(static_cast<long long>(SOL_ALG), 279LL);
  EXPECT_EQ(static_cast<long long>(SOL_NFC), 280LL);
  EXPECT_EQ(static_cast<long long>(SOL_KCM), 281LL);
  EXPECT_EQ(static_cast<long long>(SOL_TLS), 282LL);
  EXPECT_EQ(static_cast<long long>(SOL_XDP), 283LL);
  EXPECT_EQ(static_cast<long long>(SOL_MPTCP), 284LL);
  EXPECT_EQ(static_cast<long long>(SOL_MCTP), 285LL);
  EXPECT_EQ(static_cast<long long>(SOL_SMC), 286LL);
  EXPECT_EQ(static_cast<long long>(SOL_VSOCK), 287LL);
}

TEST(LlvmLibcLinuxSysSocketMacrosTest,
     DefineAf_ConstantsThroughPf_OnesAsGlibcD) {
  EXPECT_EQ(static_cast<long long>(PF_UNSPEC), 0LL);
  EXPECT_EQ(static_cast<long long>(PF_UNIX), 1LL);
  EXPECT_EQ(static_cast<long long>(PF_LOCAL), 1LL);
  EXPECT_EQ(static_cast<long long>(PF_INET), 2LL);
  EXPECT_EQ(static_cast<long long>(PF_INET6), 10LL);
  EXPECT_EQ(static_cast<long long>(PF_NETLINK), 16LL);
  EXPECT_EQ(static_cast<long long>(PF_BRIDGE), 7LL);
  EXPECT_EQ(static_cast<long long>(PF_PACKET), 17LL);
  EXPECT_EQ(static_cast<long long>(PF_VSOCK), 40LL);
  EXPECT_EQ(static_cast<long long>(PF_MPLS), 28LL);
  EXPECT_EQ(static_cast<long long>(PF_CAN), 29LL);
  EXPECT_EQ(static_cast<long long>(PF_ALG), 38LL);
  EXPECT_EQ(static_cast<long long>(PF_XDP), 44LL);
  EXPECT_EQ(static_cast<long long>(PF_AX25), 3LL);
  EXPECT_EQ(static_cast<long long>(PF_IPX), 4LL);
  EXPECT_EQ(static_cast<long long>(PF_APPLETALK), 5LL);
  EXPECT_EQ(static_cast<long long>(PF_NETROM), 6LL);
  EXPECT_EQ(static_cast<long long>(PF_ATMPVC), 8LL);
  EXPECT_EQ(static_cast<long long>(PF_X25), 9LL);
  EXPECT_EQ(static_cast<long long>(PF_ROSE), 11LL);
  EXPECT_EQ(static_cast<long long>(PF_DECnet), 12LL);
  EXPECT_EQ(static_cast<long long>(PF_NETBEUI), 13LL);
  EXPECT_EQ(static_cast<long long>(PF_SECURITY), 14LL);
  EXPECT_EQ(static_cast<long long>(PF_KEY), 15LL);
  EXPECT_EQ(static_cast<long long>(PF_ASH), 18LL);
  EXPECT_EQ(static_cast<long long>(PF_ECONET), 19LL);
  EXPECT_EQ(static_cast<long long>(PF_ATMSVC), 20LL);
  EXPECT_EQ(static_cast<long long>(PF_RDS), 21LL);
  EXPECT_EQ(static_cast<long long>(PF_SNA), 22LL);
  EXPECT_EQ(static_cast<long long>(PF_IRDA), 23LL);
  EXPECT_EQ(static_cast<long long>(PF_PPPOX), 24LL);
  EXPECT_EQ(static_cast<long long>(PF_WANPIPE), 25LL);
  EXPECT_EQ(static_cast<long long>(PF_LLC), 26LL);
  EXPECT_EQ(static_cast<long long>(PF_IB), 27LL);
  EXPECT_EQ(static_cast<long long>(PF_TIPC), 30LL);
  EXPECT_EQ(static_cast<long long>(PF_BLUETOOTH), 31LL);
  EXPECT_EQ(static_cast<long long>(PF_IUCV), 32LL);
  EXPECT_EQ(static_cast<long long>(PF_RXRPC), 33LL);
  EXPECT_EQ(static_cast<long long>(PF_ISDN), 34LL);
  EXPECT_EQ(static_cast<long long>(PF_PHONET), 35LL);
  EXPECT_EQ(static_cast<long long>(PF_IEEE802154), 36LL);
  EXPECT_EQ(static_cast<long long>(PF_CAIF), 37LL);
  EXPECT_EQ(static_cast<long long>(PF_NFC), 39LL);
  EXPECT_EQ(static_cast<long long>(PF_KCM), 41LL);
  EXPECT_EQ(static_cast<long long>(PF_QIPCRTR), 42LL);
  EXPECT_EQ(static_cast<long long>(PF_SMC), 43LL);
  EXPECT_EQ(static_cast<long long>(PF_MCTP), 45LL);
  EXPECT_EQ(static_cast<long long>(PF_MAX), 46LL);
  EXPECT_EQ(static_cast<long long>(PF_FILE), 1LL);
  EXPECT_EQ(static_cast<long long>(PF_ROUTE), 16LL);
  EXPECT_EQ(static_cast<long long>(AF_UNSPEC), 0LL);
  EXPECT_EQ(static_cast<long long>(AF_UNIX), 1LL);
  EXPECT_EQ(static_cast<long long>(AF_LOCAL), 1LL);
  EXPECT_EQ(static_cast<long long>(AF_INET), 2LL);
  EXPECT_EQ(static_cast<long long>(AF_INET6), 10LL);
  EXPECT_EQ(static_cast<long long>(AF_NETLINK), 16LL);
  EXPECT_EQ(static_cast<long long>(AF_BRIDGE), 7LL);
  EXPECT_EQ(static_cast<long long>(AF_PACKET), 17LL);
  EXPECT_EQ(static_cast<long long>(AF_VSOCK), 40LL);
  EXPECT_EQ(static_cast<long long>(AF_MPLS), 28LL);
  EXPECT_EQ(static_cast<long long>(AF_CAN), 29LL);
  EXPECT_EQ(static_cast<long long>(AF_ALG), 38LL);
  EXPECT_EQ(static_cast<long long>(AF_XDP), 44LL);
  EXPECT_EQ(static_cast<long long>(AF_AX25), 3LL);
  EXPECT_EQ(static_cast<long long>(AF_IPX), 4LL);
  EXPECT_EQ(static_cast<long long>(AF_APPLETALK), 5LL);
  EXPECT_EQ(static_cast<long long>(AF_NETROM), 6LL);
  EXPECT_EQ(static_cast<long long>(AF_ATMPVC), 8LL);
  EXPECT_EQ(static_cast<long long>(AF_X25), 9LL);
  EXPECT_EQ(static_cast<long long>(AF_ROSE), 11LL);
  EXPECT_EQ(static_cast<long long>(AF_DECnet), 12LL);
  EXPECT_EQ(static_cast<long long>(AF_NETBEUI), 13LL);
  EXPECT_EQ(static_cast<long long>(AF_SECURITY), 14LL);
  EXPECT_EQ(static_cast<long long>(AF_KEY), 15LL);
  EXPECT_EQ(static_cast<long long>(AF_ASH), 18LL);
  EXPECT_EQ(static_cast<long long>(AF_ECONET), 19LL);
  EXPECT_EQ(static_cast<long long>(AF_ATMSVC), 20LL);
  EXPECT_EQ(static_cast<long long>(AF_RDS), 21LL);
  EXPECT_EQ(static_cast<long long>(AF_SNA), 22LL);
  EXPECT_EQ(static_cast<long long>(AF_IRDA), 23LL);
  EXPECT_EQ(static_cast<long long>(AF_PPPOX), 24LL);
  EXPECT_EQ(static_cast<long long>(AF_WANPIPE), 25LL);
  EXPECT_EQ(static_cast<long long>(AF_LLC), 26LL);
  EXPECT_EQ(static_cast<long long>(AF_IB), 27LL);
  EXPECT_EQ(static_cast<long long>(AF_TIPC), 30LL);
  EXPECT_EQ(static_cast<long long>(AF_BLUETOOTH), 31LL);
  EXPECT_EQ(static_cast<long long>(AF_IUCV), 32LL);
  EXPECT_EQ(static_cast<long long>(AF_RXRPC), 33LL);
  EXPECT_EQ(static_cast<long long>(AF_ISDN), 34LL);
  EXPECT_EQ(static_cast<long long>(AF_PHONET), 35LL);
  EXPECT_EQ(static_cast<long long>(AF_IEEE802154), 36LL);
  EXPECT_EQ(static_cast<long long>(AF_CAIF), 37LL);
  EXPECT_EQ(static_cast<long long>(AF_NFC), 39LL);
  EXPECT_EQ(static_cast<long long>(AF_KCM), 41LL);
  EXPECT_EQ(static_cast<long long>(AF_QIPCRTR), 42LL);
  EXPECT_EQ(static_cast<long long>(AF_SMC), 43LL);
  EXPECT_EQ(static_cast<long long>(AF_MCTP), 45LL);
  EXPECT_EQ(static_cast<long long>(AF_MAX), 46LL);
  EXPECT_EQ(static_cast<long long>(AF_FILE), 1LL);
  EXPECT_EQ(static_cast<long long>(AF_ROUTE), 16LL);
}

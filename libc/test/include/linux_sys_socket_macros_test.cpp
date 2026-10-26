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
  EXPECT_EQ(static_cast<long long>(SCM_TIMESTAMP), 63LL);
  EXPECT_EQ(static_cast<long long>(SCM_TIMESTAMPNS), 64LL);
  EXPECT_EQ(static_cast<long long>(SCM_TIMESTAMPING), 65LL);
}

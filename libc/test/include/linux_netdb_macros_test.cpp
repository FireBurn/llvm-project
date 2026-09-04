//===-- Unittests for the linux/netdb macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the linux netdb macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/netdb-macros.h"

TEST(LlvmLibcLinuxNetdbMacrosTest, AddTheAi_FlagsToNetdbH) {
  EXPECT_EQ(static_cast<long long>(EAI_NODATA), -5LL);
  EXPECT_EQ(static_cast<long long>(EAI_ADDRFAMILY), -9LL);
  EXPECT_EQ(static_cast<long long>(NI_NUMERICHOST), 1LL);
  EXPECT_EQ(static_cast<long long>(NI_NUMERICSERV), 2LL);
  EXPECT_EQ(static_cast<long long>(NI_NOFQDN), 4LL);
  EXPECT_EQ(static_cast<long long>(NI_NAMEREQD), 8LL);
  EXPECT_EQ(static_cast<long long>(NI_DGRAM), 16LL);
  EXPECT_EQ(static_cast<long long>(NI_MAXHOST), 1025LL);
  EXPECT_EQ(static_cast<long long>(NI_MAXSERV), 32LL);
  EXPECT_EQ(static_cast<long long>(HOST_NOT_FOUND), 1LL);
  EXPECT_EQ(static_cast<long long>(TRY_AGAIN), 2LL);
  EXPECT_EQ(static_cast<long long>(NO_RECOVERY), 3LL);
  EXPECT_EQ(static_cast<long long>(NO_DATA), 4LL);
  EXPECT_EQ(static_cast<long long>(NO_ADDRESS), 4LL);
}

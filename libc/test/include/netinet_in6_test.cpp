//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public netinet/in6.h header.
///
//===----------------------------------------------------------------------===//

#include "include/netinet/in6.h"
#include "test/UnitTest/Test.h"

// Code written for bionic includes this header for what netinet/in.h holds.
TEST(LlvmLibcNetinetIn6Test, HasWhatNetinetInHas) {
  struct in6_addr address = IN6ADDR_LOOPBACK_INIT;
  struct sockaddr_in6 socket_address = {};
  EXPECT_EQ(sizeof(address), size_t(16));
  EXPECT_GE(sizeof(socket_address), sizeof(address));
  EXPECT_EQ(IPPROTO_IPV6, 41);
}

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for netdb.h bringing in netinet/in.h
///
//===----------------------------------------------------------------------===//

#include "include/netdb.h"
#include "test/UnitTest/Test.h"

#ifndef _LLVM_LIBC_NETINET_IN_H
#error "<netdb.h> does not bring in <netinet/in.h>"
#endif

TEST(LlvmLibcNetdbNetinetInTest, CarriesTheHeaders) {
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  EXPECT_EQ(address.sin_family, sa_family_t(AF_INET));
}

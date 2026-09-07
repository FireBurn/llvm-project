//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the well known ports in netinet/in.h
///
//===----------------------------------------------------------------------===//

#include "include/netinet/in.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcNetinetInPortsTest, WellKnownPorts) {
  EXPECT_EQ(int(IPPORT_ECHO), 7);
  EXPECT_EQ(int(IPPORT_DISCARD), 9);
  EXPECT_EQ(int(IPPORT_RESERVED), 1024);
  EXPECT_EQ(int(IPPORT_USERRESERVED), 5000);
}

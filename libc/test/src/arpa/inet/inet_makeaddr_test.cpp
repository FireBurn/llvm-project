//===-- Unittests for inet_makeaddr ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/in_addr_t.h"
#include "hdr/types/struct_in_addr.h"
#include "src/arpa/inet/htonl.h"
#include "src/arpa/inet/inet_makeaddr.h"
#include "src/arpa/inet/inet_network.h"
#include "test/UnitTest/Test.h"

namespace LIBC_NAMESPACE_DECL {

TEST(LlvmLibcInetMakeaddr, TheNetworkNumberSetsTheClass) {
  // Class A leaves 24 bits for the host, B 16 and C 8.
  EXPECT_EQ(htonl(0x7f000001), inet_makeaddr(0x7f, 1).s_addr);
  EXPECT_EQ(htonl(0x7f000001), inet_makeaddr(0x7f, 0x01000001).s_addr);
  EXPECT_EQ(htonl(0x00800001), inet_makeaddr(0x80, 1).s_addr);
  EXPECT_EQ(htonl(0xac100203), inet_makeaddr(0xac10, 0x10203).s_addr);
  EXPECT_EQ(htonl(0xffffffff), inet_makeaddr(0xffff, 0xffff).s_addr);
  EXPECT_EQ(htonl(0x010000ff), inet_makeaddr(0x10000, 0x1ff).s_addr);
  EXPECT_EQ(htonl(0xc0a80102), inet_makeaddr(0xc0a801, 0x102).s_addr);
  // A number already the width of an address is taken as one.
  EXPECT_EQ(htonl(0x01000005), inet_makeaddr(0x01000000, 5).s_addr);
  EXPECT_EQ(htonl(0x7f000001), inet_makeaddr(0x7f000000, 1).s_addr);
  EXPECT_EQ(htonl(0), inet_makeaddr(0, 0).s_addr);
}

TEST(LlvmLibcInetMakeaddr, PutsBackWhatInetNetworkRead) {
  EXPECT_EQ(htonl(0xc0a80102),
            inet_makeaddr(inet_network("192.168.1"), 2).s_addr);
  EXPECT_EQ(htonl(0x0a000000), inet_makeaddr(inet_network("10"), 0).s_addr);
}

} // namespace LIBC_NAMESPACE_DECL

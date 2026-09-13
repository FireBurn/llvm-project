//===-- Unittests for inet_network ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/in_addr_t.h"
#include "src/arpa/inet/inet_network.h"
#include "test/UnitTest/Test.h"

namespace LIBC_NAMESPACE_DECL {

TEST(LlvmLibcInetNetwork, PartsFillFromTheLowEnd) {
  EXPECT_EQ(in_addr_t(0x0a010203), inet_network("10.1.2.3"));
  EXPECT_EQ(in_addr_t(0x000a0102), inet_network("10.1.2"));
  EXPECT_EQ(in_addr_t(0x00000a01), inet_network("10.1"));
  EXPECT_EQ(in_addr_t(0x0000000a), inet_network("10"));
  EXPECT_EQ(in_addr_t(0xfffffffe), inet_network("0xff.0xff.0xff.0xfe"));
}

TEST(LlvmLibcInetNetwork, PartsAreCNumbers) {
  EXPECT_EQ(in_addr_t(0x7f01), inet_network("0x7f.1"));
  EXPECT_EQ(in_addr_t(0x7f01), inet_network("0177.1"));
  EXPECT_EQ(in_addr_t(0x1f), inet_network("0X1F"));
  EXPECT_EQ(in_addr_t(0x0102), inet_network("1.02"));
  EXPECT_EQ(in_addr_t(0x0102), inet_network("1.0x2"));
  EXPECT_EQ(in_addr_t(0), inet_network("0"));
  EXPECT_EQ(in_addr_t(0), inet_network("00"));
  EXPECT_EQ(in_addr_t(0), inet_network("0x0.0x0"));
}

TEST(LlvmLibcInetNetwork, SpaceMayFollow) {
  EXPECT_EQ(in_addr_t(1), inet_network("1 "));
  EXPECT_EQ(in_addr_t(1), inet_network("1\t"));
  EXPECT_EQ(in_addr_t(1), inet_network("1\n"));
  EXPECT_EQ(in_addr_t(0x0102), inet_network("1.2 \t "));
}

TEST(LlvmLibcInetNetwork, RejectsWhatIsNotANetworkNumber) {
  const in_addr_t NONE = 0xffffffff;
  // Each part is a byte.
  EXPECT_EQ(NONE, inet_network("256"));
  EXPECT_EQ(NONE, inet_network("1.256"));
  EXPECT_EQ(NONE, inet_network("0x100"));
  EXPECT_EQ(NONE, inet_network("0400"));
  EXPECT_EQ(NONE, inet_network("4294967297"));
  // There are at most four of them, and none is empty.
  EXPECT_EQ(NONE, inet_network("1.2.3.4.5"));
  EXPECT_EQ(NONE, inet_network(""));
  EXPECT_EQ(NONE, inet_network("."));
  EXPECT_EQ(NONE, inet_network("1."));
  EXPECT_EQ(NONE, inet_network(".1"));
  EXPECT_EQ(NONE, inet_network("1..2"));
  // A part is a C number, with its digits.
  EXPECT_EQ(NONE, inet_network("0x"));
  EXPECT_EQ(NONE, inet_network("x1"));
  EXPECT_EQ(NONE, inet_network("08"));
  EXPECT_EQ(NONE, inet_network("0b1"));
  EXPECT_EQ(NONE, inet_network("abc"));
  EXPECT_EQ(NONE, inet_network("-1"));
  EXPECT_EQ(NONE, inet_network("+1"));
  EXPECT_EQ(NONE, inet_network(" 1"));
  // Space ends the number.
  EXPECT_EQ(NONE, inet_network("1.2.3.4x"));
  EXPECT_EQ(NONE, inet_network("1 x"));
  EXPECT_EQ(NONE, inet_network("1.2 3"));
}

} // namespace LIBC_NAMESPACE_DECL

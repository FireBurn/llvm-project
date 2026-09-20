//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the nameser byte accessors
///
//===----------------------------------------------------------------------===//

#include "include/arpa/nameser.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcArpaNameserAccessorsTest, BytesMoveInNetworkOrder) {
  unsigned char buffer[8] = {};
  unsigned char *put = buffer;
  NS_PUT16(0x1234, put);
  NS_PUT32(0x01020304u, put);
  EXPECT_EQ(static_cast<long long>(put - buffer), static_cast<long long>(6));
  EXPECT_EQ(static_cast<long long>(int(buffer[0])), static_cast<long long>(0x12));
  EXPECT_EQ(static_cast<long long>(int(buffer[1])), static_cast<long long>(0x34));
  EXPECT_EQ(static_cast<long long>(int(buffer[2])), static_cast<long long>(0x01));
  EXPECT_EQ(static_cast<long long>(int(buffer[5])), static_cast<long long>(0x04));

  const unsigned char *get = buffer;
  unsigned short s;
  unsigned int l;
  NS_GET16(s, get);
  NS_GET32(l, get);
  EXPECT_EQ(static_cast<long long>(s), static_cast<long long>(0x1234));
  EXPECT_EQ(static_cast<long long>(l), static_cast<long long>(0x01020304u));
  EXPECT_EQ(static_cast<long long>(get - buffer), static_cast<long long>(6));
}

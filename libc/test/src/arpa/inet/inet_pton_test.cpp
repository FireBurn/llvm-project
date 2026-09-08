//===-- Unittests for inet_pton -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/sys_socket_macros.h"
#include "src/arpa/inet/inet_pton.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcInetPtonTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {

bool v4_is(const char *text, unsigned char a, unsigned char b, unsigned char c,
           unsigned char d) {
  unsigned char got[4] = {};
  if (LIBC_NAMESPACE::inet_pton(AF_INET, text, got) != 1)
    return false;
  return got[0] == a && got[1] == b && got[2] == c && got[3] == d;
}

bool v6_is(const char *text, const unsigned char want[16]) {
  unsigned char got[16] = {};
  if (LIBC_NAMESPACE::inet_pton(AF_INET6, text, got) != 1)
    return false;
  for (int i = 0; i < 16; ++i)
    if (got[i] != want[i])
      return false;
  return true;
}

} // anonymous namespace

TEST_F(LlvmLibcInetPtonTest, IPv4) {
  ASSERT_TRUE(v4_is("1.2.3.4", 1, 2, 3, 4));
  ASSERT_TRUE(v4_is("0.0.0.0", 0, 0, 0, 0));
  ASSERT_TRUE(v4_is("255.255.255.255", 255, 255, 255, 255));
  ASSERT_TRUE(v4_is("127.0.0.1", 127, 0, 0, 1));
}

TEST_F(LlvmLibcInetPtonTest, IPv4Rejects) {
  unsigned char got[4];
  // Only the four part dotted form, in decimal, no leading zeros.
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_INET, "1.2.3", got), 0);
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_INET, "1.2.3.4.5", got), 0);
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_INET, "256.1.1.1", got), 0);
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_INET, "01.2.3.4", got), 0);
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_INET, "010.1.1.1", got), 0);
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_INET, "0x1.2.3.4", got), 0);
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_INET, "1.2.3.4 ", got), 0);
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_INET, "", got), 0);
}

TEST_F(LlvmLibcInetPtonTest, IPv6) {
  const unsigned char zero[16] = {};
  ASSERT_TRUE(v6_is("::", zero));

  const unsigned char one[16] = {0, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0, 0, 1};
  ASSERT_TRUE(v6_is("::1", one));

  const unsigned char full[16] = {0, 1, 0, 2, 0, 3, 0, 4,
                                  0, 5, 0, 6, 0, 7, 0, 8};
  ASSERT_TRUE(v6_is("1:2:3:4:5:6:7:8", full));

  // The same address written out and with the zeros left out.
  const unsigned char db8[16] = {0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0,
                                 0,    0,    0,    0,    0, 0, 0, 1};
  ASSERT_TRUE(v6_is("2001:db8::1", db8));
  ASSERT_TRUE(v6_is("2001:0db8:0000:0000:0000:0000:0000:0001", db8));

  // A run left out at either end.
  const unsigned char lead[16] = {0, 1, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0};
  ASSERT_TRUE(v6_is("1::", lead));
}

TEST_F(LlvmLibcInetPtonTest, IPv6WithEmbeddedIPv4) {
  const unsigned char mapped[16] = {0, 0, 0,    0,    0, 0, 0, 0,
                                    0, 0, 0xff, 0xff, 1, 2, 3, 4};
  ASSERT_TRUE(v6_is("::ffff:1.2.3.4", mapped));

  const unsigned char compat[16] = {0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 1, 2, 3, 4};
  ASSERT_TRUE(v6_is("::1.2.3.4", compat));

  const unsigned char six[16] = {0, 1, 0, 2, 0, 3, 0, 4,
                                 0, 5, 0, 6, 1, 2, 3, 4};
  ASSERT_TRUE(v6_is("1:2:3:4:5:6:1.2.3.4", six));
}

TEST_F(LlvmLibcInetPtonTest, IPv6Rejects) {
  unsigned char got[16];
  // Two runs left out, too few groups, too many, a group too wide, a digit
  // which is not hex, and a colon with nothing after it.
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_INET6, "1::2::3", got), 0);
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_INET6, "1:2:3:4:5:6:7", got), 0);
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_INET6, "1:2:3:4:5:6:7:8:9", got), 0);
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_INET6, "12345::", got), 0);
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_INET6, "g::1", got), 0);
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_INET6, ":", got), 0);
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_INET6, "1:2:3:4:5:6:7:", got), 0);
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_INET6, "::ffff:256.1.1.1", got), 0);
  // A scope is not part of the address.
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_INET6, "fe80::1%eth0", got), 0);
}

TEST_F(LlvmLibcInetPtonTest, UnknownFamily) {
  unsigned char got[16];
  ASSERT_EQ(LIBC_NAMESPACE::inet_pton(AF_UNIX, "1.2.3.4", got), -1);
  ASSERT_ERRNO_EQ(EAFNOSUPPORT);
}

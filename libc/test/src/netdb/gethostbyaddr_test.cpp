//===-- Unittests for gethostbyaddr ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/sys_socket_macros.h"
#include "hdr/types/struct_hostent.h"
#include "src/netdb/gethostbyaddr.h"
#include "test/UnitTest/Test.h"

// An address of the wrong length for the family it claims names nothing.
TEST(LlvmLibcGetHostByAddrTest, TheLengthMustSuitTheFamily) {
  const unsigned char four[4] = {127, 0, 0, 1};
  ASSERT_TRUE(LIBC_NAMESPACE::gethostbyaddr(four, 3, AF_INET) == nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::gethostbyaddr(four, 16, AF_INET) == nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::gethostbyaddr(four, 4, AF_INET6) == nullptr);
}

TEST(LlvmLibcGetHostByAddrTest, NoAddressIsNoAnswer) {
  ASSERT_TRUE(LIBC_NAMESPACE::gethostbyaddr(nullptr, 4, AF_INET) == nullptr);
}

// A family the addresses are not written in names nothing either.
TEST(LlvmLibcGetHostByAddrTest, OnlyInternetFamilies) {
  const unsigned char four[4] = {127, 0, 0, 1};
  ASSERT_TRUE(LIBC_NAMESPACE::gethostbyaddr(four, 4, AF_UNIX) == nullptr);
}

// Where the address does have a name, what comes back describes the address
// that was asked about.
TEST(LlvmLibcGetHostByAddrTest, TheAnswerDescribesTheAddress) {
  const unsigned char loopback[4] = {127, 0, 0, 1};
  struct hostent *entry = LIBC_NAMESPACE::gethostbyaddr(loopback, 4, AF_INET);
  if (entry == nullptr)
    return; // Nothing names it here, which is allowed.
  ASSERT_EQ(entry->h_addrtype, AF_INET);
  ASSERT_EQ(entry->h_length, 4);
  ASSERT_TRUE(entry->h_name != nullptr);
  ASSERT_TRUE(entry->h_addr_list[0] != nullptr);
  ASSERT_TRUE(entry->h_addr_list[1] == nullptr);
  for (int i = 0; i < 4; ++i)
    ASSERT_EQ(static_cast<unsigned char>(entry->h_addr_list[0][i]),
              loopback[i]);
}

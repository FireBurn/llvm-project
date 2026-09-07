//===-- Unittests for the networks database -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/sys_socket_macros.h"
#include "hdr/types/struct_netent.h"
#include "src/netdb/endnetent.h"
#include "src/netdb/getnetbyaddr.h"
#include "src/netdb/getnetbyname.h"
#include "src/netdb/getnetent.h"
#include "src/netdb/setnetent.h"
#include "test/UnitTest/Test.h"

// What is read comes from /etc/networks, whose contents are the system's to
// decide, so these say what has to hold of whatever it names.

TEST(LlvmLibcGetNetEntTest, NoNameIsNoAnswer) {
  ASSERT_TRUE(LIBC_NAMESPACE::getnetbyname(nullptr) == nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::getnetbyname("no such network") == nullptr);
}

// Only internet networks are named this way, so anything else finds nothing.
TEST(LlvmLibcGetNetEntTest, OnlyInternetAddressesAreFound) {
  ASSERT_TRUE(LIBC_NAMESPACE::getnetbyaddr(0x7f000000, AF_INET6) == nullptr);
}

// Where the file names a network, the two ways of asking agree.
TEST(LlvmLibcGetNetEntTest, NameAndNumberAgree) {
  struct netent *by_name = LIBC_NAMESPACE::getnetbyname("loopback");
  if (by_name == nullptr)
    return;
  ASSERT_EQ(by_name->n_addrtype, AF_INET);
  const uint32_t net = by_name->n_net;

  struct netent *by_addr = LIBC_NAMESPACE::getnetbyaddr(net, AF_INET);
  ASSERT_TRUE(by_addr != nullptr);
  ASSERT_EQ(by_addr->n_net, net);
  ASSERT_STREQ(by_addr->n_name, "loopback");
}

// Reading through ends, and starting again gives the same first entry.
TEST(LlvmLibcGetNetEntTest, EnumerationStartsAgainFromTheFront) {
  LIBC_NAMESPACE::setnetent(1);
  struct netent *first = LIBC_NAMESPACE::getnetent();
  if (first == nullptr) {
    LIBC_NAMESPACE::endnetent();
    return;
  }
  const uint32_t first_net = first->n_net;

  int seen = 1;
  while (LIBC_NAMESPACE::getnetent() != nullptr && seen < 4096)
    ++seen;
  ASSERT_LT(seen, 4096);
  LIBC_NAMESPACE::endnetent();

  LIBC_NAMESPACE::setnetent(0);
  struct netent *again = LIBC_NAMESPACE::getnetent();
  ASSERT_TRUE(again != nullptr);
  ASSERT_EQ(again->n_net, first_net);
  LIBC_NAMESPACE::endnetent();
}

TEST(LlvmLibcGetNetEntTest, EndingWithoutStartingIsHarmless) {
  LIBC_NAMESPACE::endnetent();
  LIBC_NAMESPACE::endnetent();
}

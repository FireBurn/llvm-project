//===-- Unittests for the services database -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/struct_servent.h"
#include "src/netdb/endservent.h"
#include "src/netdb/getservbyname.h"
#include "src/netdb/getservbyport.h"
#include "src/netdb/getservent.h"
#include "src/netdb/setservent.h"
#include "test/UnitTest/Test.h"

// What is read comes from /etc/services, so these say what has to hold of
// whatever it names rather than assuming any particular service is there.

TEST(LlvmLibcGetServEntTest, NoNameIsNoAnswer) {
  ASSERT_TRUE(LIBC_NAMESPACE::getservbyname(nullptr, nullptr) == nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::getservbyname("no such service", nullptr) ==
              nullptr);
}

// The two ways of asking about the same service agree.
TEST(LlvmLibcGetServEntTest, NameAndPortAgree) {
  struct servent *by_name = LIBC_NAMESPACE::getservbyname("ssh", "tcp");
  if (by_name == nullptr)
    return;
  ASSERT_STREQ(by_name->s_name, "ssh");
  ASSERT_STREQ(by_name->s_proto, "tcp");
  const int port = by_name->s_port;

  struct servent *by_port = LIBC_NAMESPACE::getservbyport(port, "tcp");
  ASSERT_TRUE(by_port != nullptr);
  ASSERT_EQ(by_port->s_port, port);
  ASSERT_STREQ(by_port->s_name, "ssh");
}

// Asking for one protocol does not answer with another.
TEST(LlvmLibcGetServEntTest, TheProtocolIsHonoured) {
  struct servent *entry = LIBC_NAMESPACE::getservbyname("ssh", "udp");
  if (entry != nullptr)
    ASSERT_STREQ(entry->s_proto, "udp");
}

// The other names a service goes by are reachable.
TEST(LlvmLibcGetServEntTest, AliasesAreReported) {
  struct servent *entry = LIBC_NAMESPACE::getservbyname("http", "tcp");
  if (entry == nullptr)
    return;
  ASSERT_TRUE(entry->s_aliases != nullptr);
  size_t count = 0;
  while (entry->s_aliases[count] != nullptr)
    ++count;
  // The list ends, whatever is in it.
  ASSERT_LT(count, size_t(64));
}

TEST(LlvmLibcGetServEntTest, EnumerationStartsAgainFromTheFront) {
  LIBC_NAMESPACE::setservent(1);
  struct servent *first = LIBC_NAMESPACE::getservent();
  if (first == nullptr) {
    LIBC_NAMESPACE::endservent();
    return;
  }
  const int first_port = first->s_port;
  LIBC_NAMESPACE::endservent();

  LIBC_NAMESPACE::setservent(0);
  struct servent *again = LIBC_NAMESPACE::getservent();
  ASSERT_TRUE(again != nullptr);
  ASSERT_EQ(again->s_port, first_port);
  LIBC_NAMESPACE::endservent();
}

TEST(LlvmLibcGetServEntTest, EndingWithoutStartingIsHarmless) {
  LIBC_NAMESPACE::endservent();
  LIBC_NAMESPACE::endservent();
}

//===-- Unittests for the hosts database ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/sys_socket_macros.h"
#include "hdr/types/struct_hostent.h"
#include "src/netdb/endhostent.h"
#include "src/netdb/gethostent.h"
#include "src/netdb/sethostent.h"
#include "test/UnitTest/Test.h"

// What is read comes from /etc/hosts, whose contents are the system's to
// decide. The tests below say what has to hold of whatever it names, rather
// than assuming any particular entry is there.

// Walking the file hands back IPv4 entries and nothing else, since the
// interface gives no way to ask for a family.
TEST(LlvmLibcGetHostEntTest, EveryEntryIsAnAddressOfFourOctets) {
  LIBC_NAMESPACE::sethostent(0);
  int seen = 0;
  for (struct hostent *entry = LIBC_NAMESPACE::gethostent();
       entry != nullptr && seen < 4096;
       entry = LIBC_NAMESPACE::gethostent(), ++seen) {
    ASSERT_EQ(entry->h_addrtype, AF_INET);
    ASSERT_EQ(entry->h_length, 4);
    ASSERT_TRUE(entry->h_name != nullptr);
    ASSERT_TRUE(entry->h_aliases != nullptr);
    ASSERT_TRUE(entry->h_addr_list != nullptr);
    ASSERT_TRUE(entry->h_addr_list[0] != nullptr);
    // One line names one address, so the list holds one.
    ASSERT_TRUE(entry->h_addr_list[1] == nullptr);
  }
  ASSERT_LT(seen, 4096);
  LIBC_NAMESPACE::endhostent();
}

TEST(LlvmLibcGetHostEntTest, TheEndStaysReachedUntilAskedToStartAgain) {
  LIBC_NAMESPACE::sethostent(1);
  struct hostent *first = LIBC_NAMESPACE::gethostent();
  if (first == nullptr) {
    LIBC_NAMESPACE::endhostent();
    return;
  }
  const int first_type = first->h_addrtype;

  int seen = 1;
  while (LIBC_NAMESPACE::gethostent() != nullptr && seen < 4096)
    ++seen;
  ASSERT_LT(seen, 4096);

  // Reading past the end says there is nothing rather than quietly starting
  // over, which would leave a caller looping forever.
  ASSERT_TRUE(LIBC_NAMESPACE::gethostent() == nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::gethostent() == nullptr);

  LIBC_NAMESPACE::sethostent(0);
  struct hostent *again = LIBC_NAMESPACE::gethostent();
  ASSERT_TRUE(again != nullptr);
  ASSERT_EQ(again->h_addrtype, first_type);
  LIBC_NAMESPACE::endhostent();
}

// Ending an enumeration that was never started is harmless.
TEST(LlvmLibcGetHostEntTest, EndingWithoutStartingIsHarmless) {
  LIBC_NAMESPACE::endhostent();
  LIBC_NAMESPACE::endhostent();
}

//===-- Unittests for getifaddrs ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/net_if_macros.h"
#include "hdr/sys_socket_macros.h"
#include "hdr/types/struct_ifaddrs.h"
#include "src/ifaddrs/freeifaddrs.h"
#include "src/ifaddrs/getifaddrs.h"
#include "src/string/strcmp.h"
#include "test/UnitTest/Test.h"

// Every machine has a loopback interface, so there is always something to
// find and something to check the shape of.
TEST(LlvmLibcGetifaddrsTest, FindsTheLoopbackInterface) {
  struct ifaddrs *list = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::getifaddrs(&list), 0);
  ASSERT_TRUE(list != nullptr);

  bool saw_loopback_address = false;
  bool saw_link_entry = false;
  size_t count = 0;
  for (struct ifaddrs *entry = list; entry != nullptr;
       entry = entry->ifa_next) {
    ++count;
    ASSERT_TRUE(entry->ifa_name != nullptr);
    // Every entry has an address; that is what it is an entry for.
    ASSERT_TRUE(entry->ifa_addr != nullptr);

    if ((entry->ifa_flags & IFF_LOOPBACK) != 0) {
      if (entry->ifa_addr->sa_family == AF_INET ||
          entry->ifa_addr->sa_family == AF_INET6) {
        saw_loopback_address = true;
        // An internet address always comes with the netmask that goes with it.
        ASSERT_TRUE(entry->ifa_netmask != nullptr);
        ASSERT_EQ(int(entry->ifa_netmask->sa_family),
                  int(entry->ifa_addr->sa_family));
      }
    }
    if (entry->ifa_addr->sa_family == AF_PACKET) {
      saw_link_entry = true;
      // The link entry is the interface itself and has no netmask.
      ASSERT_TRUE(entry->ifa_netmask == nullptr);
    }
  }

  ASSERT_TRUE(count > 0);
  ASSERT_TRUE(saw_loopback_address);
  ASSERT_TRUE(saw_link_entry);

  LIBC_NAMESPACE::freeifaddrs(list);
}

TEST(LlvmLibcGetifaddrsTest, RejectsSomewhereToPutNothing) {
  ASSERT_EQ(LIBC_NAMESPACE::getifaddrs(nullptr), -1);
}

TEST(LlvmLibcGetifaddrsTest, FreeingNothingIsAllowed) {
  // What a caller does after a failure, where the list was never set.
  LIBC_NAMESPACE::freeifaddrs(nullptr);
}

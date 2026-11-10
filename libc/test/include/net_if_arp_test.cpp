//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for net/if_arp.h
///
//===----------------------------------------------------------------------===//

#include "include/llvm-libc-macros/net-if-arp-macros.h"
#include "include/llvm-libc-types/struct_arphdr.h"
#include "include/llvm-libc-types/struct_arpreq.h"
#include "include/llvm-libc-types/struct_arpreq_old.h"
#include "test/UnitTest/Test.h"

#include <stddef.h>

// The ioctls that read and change the address table are handed one of these,
// so the members have to sit where the kernel expects them.
TEST(LlvmLibcNetIfArpTest, ArpreqLayout) {
  ASSERT_EQ(offsetof(struct arpreq, arp_pa), size_t(0));
  ASSERT_EQ(offsetof(struct arpreq, arp_ha), size_t(16));
  ASSERT_EQ(offsetof(struct arpreq, arp_flags), size_t(32));
  ASSERT_EQ(offsetof(struct arpreq, arp_netmask), size_t(36));
  ASSERT_EQ(offsetof(struct arpreq, arp_dev), size_t(52));
  ASSERT_EQ(sizeof(struct arpreq), size_t(68));
}

// The older form is the same up to the interface name it does not carry.
TEST(LlvmLibcNetIfArpTest, ArpreqOldLayout) {
  ASSERT_EQ(offsetof(struct arpreq_old, arp_pa), size_t(0));
  ASSERT_EQ(offsetof(struct arpreq_old, arp_netmask), size_t(36));
  ASSERT_EQ(sizeof(struct arpreq_old), size_t(52));
}

// Each flag is one bit, and they do not overlap.
TEST(LlvmLibcNetIfArpTest, FlagsAreDistinctBits) {
  const int flags[] = {ATF_COM,     ATF_PERM,    ATF_PUBL, ATF_USETRAILERS,
                       ATF_NETMASK, ATF_DONTPUB, ATF_MAGIC};
  int seen = 0;
  for (int f : flags) {
    ASSERT_NE(f, 0);
    ASSERT_EQ(f & (f - 1), 0);
    ASSERT_EQ(seen & f, 0);
    seen |= f;
  }
}

// The fixed part of an ARP message, as the kernel lays it out.
TEST(LlvmLibcNetIfArpTest, ArphdrLayout) {
  EXPECT_EQ(sizeof(struct arphdr), size_t(8));
  EXPECT_EQ(offsetof(struct arphdr, ar_hrd), size_t(0));
  EXPECT_EQ(offsetof(struct arphdr, ar_pro), size_t(2));
  EXPECT_EQ(offsetof(struct arphdr, ar_hln), size_t(4));
  EXPECT_EQ(offsetof(struct arphdr, ar_pln), size_t(5));
  EXPECT_EQ(offsetof(struct arphdr, ar_op), size_t(6));
}

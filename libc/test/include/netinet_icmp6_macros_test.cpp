//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the neighbour discovery option numbers of netinet/icmp6.h
///
//===----------------------------------------------------------------------===//

#include "include/llvm-libc-macros/netinet-icmp6-macros.h"
#include "include/llvm-libc-types/struct_icmp6_hdr.h"
#include "test/UnitTest/Test.h"

#include <stddef.h>

// The numbers are the IANA registry's, which routers and hosts both use.
TEST(LlvmLibcNetinetIcmp6MacrosTest, OptionNumbers) {
  EXPECT_EQ(ND_OPT_SOURCE_LINKADDR, 1);
  EXPECT_EQ(ND_OPT_MTU, 5);
  EXPECT_EQ(ND_OPT_RTR_ADV_INTERVAL, 7);
  EXPECT_EQ(ND_OPT_SOURCE_ADDRLIST, 9);
  EXPECT_EQ(ND_OPT_TARGET_ADDRLIST, 10);
  EXPECT_EQ(ND_OPT_MAP, 23);
  EXPECT_EQ(ND_OPT_ROUTE_INFO, 24);
  EXPECT_EQ(ND_OPT_RDNSS, 25);
  EXPECT_EQ(ND_OPT_DNSSL, 31);
  EXPECT_EQ(ND_OPT_CAPTIVE_PORTAL, 37);
  EXPECT_EQ(ND_OPT_PREF64, 38);
}

// These are read straight off the wire, so the fields have to sit where the
// RFCs put them.
TEST(LlvmLibcNetinetIcmp6MacrosTest, NameResolutionOptionLayout) {
  EXPECT_EQ(sizeof(struct nd_opt_rdnss), size_t(8));
  EXPECT_EQ(offsetof(struct nd_opt_rdnss, nd_opt_rdnss_lifetime), size_t(4));
  EXPECT_EQ(sizeof(struct nd_opt_dnssl), size_t(8));
  EXPECT_EQ(offsetof(struct nd_opt_dnssl, nd_opt_dnssl_lifetime), size_t(4));
  EXPECT_EQ(offsetof(struct nd_opt_captive_portal, nd_opt_captive_portal_uri),
            size_t(2));
  EXPECT_EQ(offsetof(struct nd_opt_pref64, nd_opt_pref64_prefix), size_t(4));
}

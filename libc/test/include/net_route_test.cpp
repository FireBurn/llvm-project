//===-- Unittests for net/route.h -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "include/llvm-libc-macros/net-route-macros.h"
#include "include/llvm-libc-types/struct_in6_rtmsg.h"
#include "include/llvm-libc-types/struct_rtentry.h"
#include "test/UnitTest/Test.h"

#include <stddef.h>

// A route is handed to the kernel as it stands, so the members have to sit
// where the kernel reads them. The padding is what keeps them there.
TEST(LlvmLibcNetRouteTest, RtentryLayout) {
  ASSERT_EQ(offsetof(struct rtentry, rt_dst), size_t(8));
  ASSERT_EQ(offsetof(struct rtentry, rt_gateway), size_t(24));
  ASSERT_EQ(offsetof(struct rtentry, rt_genmask), size_t(40));
  ASSERT_EQ(offsetof(struct rtentry, rt_flags), size_t(56));
  ASSERT_EQ(offsetof(struct rtentry, rt_metric), size_t(80));
  ASSERT_EQ(offsetof(struct rtentry, rt_dev), size_t(88));
  ASSERT_EQ(offsetof(struct rtentry, rt_mtu), size_t(96));
  ASSERT_EQ(offsetof(struct rtentry, rt_irtt), size_t(112));
  ASSERT_EQ(sizeof(struct rtentry), size_t(120));
}

TEST(LlvmLibcNetRouteTest, In6RtmsgLayout) {
  ASSERT_EQ(offsetof(struct in6_rtmsg, rtmsg_dst), size_t(0));
  ASSERT_EQ(offsetof(struct in6_rtmsg, rtmsg_metric), size_t(56));
  ASSERT_EQ(offsetof(struct in6_rtmsg, rtmsg_info), size_t(64));
  ASSERT_EQ(offsetof(struct in6_rtmsg, rtmsg_ifindex), size_t(76));
  ASSERT_EQ(sizeof(struct in6_rtmsg), size_t(80));
}

// The flags which say what one route is are each one bit.
TEST(LlvmLibcNetRouteTest, FlagsAreDistinctBits) {
  const unsigned flags[] = {RTF_UP,     RTF_GATEWAY, RTF_HOST,
                            RTF_REJECT, RTF_STATIC,  RTF_XRESOLVE};
  unsigned seen = 0;
  for (unsigned f : flags) {
    ASSERT_NE(f, 0u);
    ASSERT_EQ(f & (f - 1), 0u);
    ASSERT_EQ(seen & f, 0u);
    seen |= f;
  }
  // The one that used to be called something else is the same bit.
  ASSERT_EQ(unsigned(RTF_MSS), unsigned(RTF_MTU));
}

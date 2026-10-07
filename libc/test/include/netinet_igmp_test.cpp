//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public netinet/igmp.h header.
///
//===----------------------------------------------------------------------===//

#include "include/netinet/igmp.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcNetinetIgmpTest, MacrosHaveTheirValues) {
  static const struct {
    const char *name;
    long long actual;
    long long expected;
  } macros[] = {
      {"IGMP_MINLEN", static_cast<long long>(IGMP_MINLEN), 8LL},
      {"IGMP_MEMBERSHIP_QUERY", static_cast<long long>(IGMP_MEMBERSHIP_QUERY), 17LL},
      {"IGMP_V1_MEMBERSHIP_REPORT", static_cast<long long>(IGMP_V1_MEMBERSHIP_REPORT), 18LL},
      {"IGMP_V2_MEMBERSHIP_REPORT", static_cast<long long>(IGMP_V2_MEMBERSHIP_REPORT), 22LL},
      {"IGMP_V2_LEAVE_GROUP", static_cast<long long>(IGMP_V2_LEAVE_GROUP), 23LL},
      {"IGMP_DVMRP", static_cast<long long>(IGMP_DVMRP), 19LL},
      {"IGMP_PIM", static_cast<long long>(IGMP_PIM), 20LL},
      {"IGMP_TRACE", static_cast<long long>(IGMP_TRACE), 21LL},
      {"IGMP_MTRACE_RESP", static_cast<long long>(IGMP_MTRACE_RESP), 30LL},
      {"IGMP_MTRACE", static_cast<long long>(IGMP_MTRACE), 31LL},
      {"IGMP_MRDISC_ADV", static_cast<long long>(IGMP_MRDISC_ADV), 48LL},
      {"IGMP_MAX_HOST_REPORT_DELAY", static_cast<long long>(IGMP_MAX_HOST_REPORT_DELAY), 10LL},
      {"IGMP_TIMER_SCALE", static_cast<long long>(IGMP_TIMER_SCALE), 10LL},
      {"IGMP_DELAYING_MEMBER", static_cast<long long>(IGMP_DELAYING_MEMBER), 1LL},
      {"IGMP_IDLE_MEMBER", static_cast<long long>(IGMP_IDLE_MEMBER), 2LL},
      {"IGMP_LAZY_MEMBER", static_cast<long long>(IGMP_LAZY_MEMBER), 3LL},
      {"IGMP_SLEEPING_MEMBER", static_cast<long long>(IGMP_SLEEPING_MEMBER), 4LL},
      {"IGMP_AWAKENING_MEMBER", static_cast<long long>(IGMP_AWAKENING_MEMBER), 5LL},
      {"IGMP_v1_ROUTER", static_cast<long long>(IGMP_v1_ROUTER), 1LL},
      {"IGMP_v2_ROUTER", static_cast<long long>(IGMP_v2_ROUTER), 2LL},
      {"IGMP_HOST_MEMBERSHIP_QUERY", static_cast<long long>(IGMP_HOST_MEMBERSHIP_QUERY), 17LL},
      {"IGMP_HOST_MEMBERSHIP_REPORT", static_cast<long long>(IGMP_HOST_MEMBERSHIP_REPORT), 18LL},
      {"IGMP_HOST_NEW_MEMBERSHIP_REPORT", static_cast<long long>(IGMP_HOST_NEW_MEMBERSHIP_REPORT), 22LL},
      {"IGMP_HOST_LEAVE_MESSAGE", static_cast<long long>(IGMP_HOST_LEAVE_MESSAGE), 23LL},
  };
  for (const auto &m : macros)
    EXPECT_EQ(m.actual, m.expected) << m.name;
}

TEST(LlvmLibcNetinetIgmpTest, TypesAreComplete) {
  EXPECT_GT(sizeof(struct igmp), size_t(0));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(struct igmp), size_t(8));
#endif
}

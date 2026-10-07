//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public netinet/ip6.h header.
///
//===----------------------------------------------------------------------===//

#include "include/netinet/ip6.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcNetinetIp6ExtensionTest, TypesAreComplete) {
  EXPECT_GT(sizeof(struct ip6_ext), size_t(0));
  EXPECT_GT(sizeof(struct ip6_dest), size_t(0));
  EXPECT_GT(sizeof(struct ip6_rthdr), size_t(0));
  EXPECT_GT(sizeof(struct ip6_frag), size_t(0));
  EXPECT_GT(sizeof(struct ip6_opt), size_t(0));
  EXPECT_GT(sizeof(struct ip6_rthdr0), size_t(0));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(struct ip6_ext), size_t(2));
  EXPECT_EQ(sizeof(struct ip6_dest), size_t(2));
  EXPECT_EQ(sizeof(struct ip6_rthdr), size_t(4));
  EXPECT_EQ(sizeof(struct ip6_frag), size_t(8));
  EXPECT_EQ(sizeof(struct ip6_opt), size_t(2));
  EXPECT_EQ(sizeof(struct ip6_rthdr0), size_t(8));
#endif
}

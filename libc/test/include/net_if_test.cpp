//===-- Unittests for net/if.h --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "include/llvm-libc-types/struct_ifreq.h"
#include "test/UnitTest/Test.h"

#include <stddef.h>

// An interface request is passed to the kernel, so the name has to come first
// and everything else has to start where the kernel reads it.
TEST(LlvmLibcNetIfTest, IfreqLayout) {
  ASSERT_EQ(offsetof(struct ifreq, ifr_ifrn), size_t(0));
  ASSERT_EQ(offsetof(struct ifreq, ifr_ifru), size_t(IF_NAMESIZE));
  ASSERT_EQ(sizeof(struct ifreq), size_t(40));
}

// The names the request is usually written with reach the same members as the
// unions they stand for.
TEST(LlvmLibcNetIfTest, ShorthandNamesReachTheUnions) {
  struct ifreq request;
  ASSERT_TRUE(request.ifr_name == request.ifr_ifrn.ifrn_name);
  ASSERT_TRUE(&request.ifr_flags == &request.ifr_ifru.ifru_flags);
  ASSERT_TRUE(&request.ifr_addr == &request.ifr_ifru.ifru_addr);
  ASSERT_TRUE(&request.ifr_mtu == &request.ifr_ifru.ifru_mtu);
  // Three of the names are the one integer the request carries.
  ASSERT_TRUE(&request.ifr_ifindex == &request.ifr_metric);
  ASSERT_TRUE(&request.ifr_ifindex == &request.ifr_qlen);
}

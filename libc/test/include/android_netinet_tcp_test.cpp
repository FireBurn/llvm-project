//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that the TCP types leave the kernel's definitions in place on Android,
/// where programs include both netinet/tcp.h and linux/tcp.h.
///
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/netinet-tcp-macros.h"
#include "include/llvm-libc-types/struct_tcp_info.h"
#include "include/llvm-libc-types/struct_tcphdr.h"

// Without the guard in the two headers above, this is a redefinition.
#include <linux/tcp.h>

#include <stddef.h>

TEST(LlvmLibcAndroidNetinetTcpTest, KernelHeaderCanBeIncludedAfterOurs) {
  EXPECT_EQ(sizeof(struct tcphdr), size_t(20));
  EXPECT_EQ(offsetof(struct tcphdr, dest), size_t(2));
  EXPECT_EQ(offsetof(struct tcphdr, ack_seq), size_t(8));
  EXPECT_GT(sizeof(struct tcp_info), size_t(0));
  EXPECT_EQ(static_cast<int>(TCP_SEND_QUEUE), 2);
}

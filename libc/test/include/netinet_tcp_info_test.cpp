//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public netinet/tcp.h header.
///
//===----------------------------------------------------------------------===//

#include "include/netinet/tcp.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcNetinetTcpInfoTest, TypesAreComplete) {
  EXPECT_GT(sizeof(struct tcp_info), size_t(0));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(struct tcp_info), size_t(280));
#endif
}

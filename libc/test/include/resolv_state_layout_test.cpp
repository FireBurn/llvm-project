//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the layout of struct __res_state
///
//===----------------------------------------------------------------------===//

#include "include/resolv.h"
#include "test/UnitTest/Test.h"

// The layouts are the ones glibc has, which programs built against it assume.
TEST(LlvmLibcResolvStateLayoutTest, MatchesGlibcOnX86_64) {
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(struct __res_state), size_t(568));
  EXPECT_EQ(__builtin_offsetof(struct __res_state, retrans), size_t(0));
  EXPECT_EQ(__builtin_offsetof(struct __res_state, retry), size_t(4));
  EXPECT_EQ(__builtin_offsetof(struct __res_state, options), size_t(8));
  EXPECT_EQ(__builtin_offsetof(struct __res_state, nscount), size_t(16));
  EXPECT_EQ(__builtin_offsetof(struct __res_state, nsaddr_list), size_t(20));
  EXPECT_EQ(__builtin_offsetof(struct __res_state, id), size_t(68));
  EXPECT_EQ(__builtin_offsetof(struct __res_state, dnsrch), size_t(72));
  EXPECT_EQ(__builtin_offsetof(struct __res_state, defdname), size_t(128));
  EXPECT_EQ(__builtin_offsetof(struct __res_state, pfcode), size_t(384));
#endif
}

//===-- Unittests for <netinet/tcp.h> -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "include/llvm-libc-macros/stdint-macros.h"
#include "include/llvm-libc-types/struct_tcphdr.h"
#include "test/UnitTest/Test.h"

// The header is twenty octets before any options.
TEST(LlvmLibcNetinetTcpTest, TheHeaderIsTheSizeItIsOnTheWire) {
  ASSERT_EQ(sizeof(struct tcphdr), size_t(20));
  ASSERT_EQ(__builtin_offsetof(struct tcphdr, th_seq), size_t(4));
  ASSERT_EQ(__builtin_offsetof(struct tcphdr, th_win), size_t(14));
  ASSERT_EQ(__builtin_offsetof(struct tcphdr, ack_seq), size_t(8));
  ASSERT_EQ(__builtin_offsetof(struct tcphdr, urg_ptr), size_t(18));
}

// The names BSD gave the fields and the ones Linux gave them are two ways of
// reaching the same octets.
TEST(LlvmLibcNetinetTcpTest, TheTwoSetsOfNamesAgree) {
  struct tcphdr header = {};
  header.th_sport = 0x1234;
  header.th_seq = 0xdeadbeef;
  header.th_off = 5;
  header.th_win = 0x4321;

  ASSERT_EQ(header.source, uint16_t(0x1234));
  ASSERT_EQ(header.seq, uint32_t(0xdeadbeef));
  ASSERT_EQ(header.doff, uint16_t(5));
  ASSERT_EQ(header.window, uint16_t(0x4321));
}

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the resolver calls that take a state of their own
///
//===----------------------------------------------------------------------===//

#include "hdr/types/struct___res_state.h"
#include "src/netdb/resolv/dns_message.h"
#include "src/resolv/res_nclose.h"
#include "src/resolv/res_ninit.h"
#include "src/resolv/res_nmkquery.h"
#include "src/resolv/res_nquery.h"
#include "src/string/memset.h"
#include "test/UnitTest/Test.h"

// Reading /etc/resolv.conf is the system's to decide, so these say only what
// has to hold whatever it contains.

TEST(LlvmLibcResNTest, InitFillsInTheStateItIsGiven) {
  struct __res_state state;
  LIBC_NAMESPACE::memset(&state, 0, sizeof(state));
  ASSERT_EQ(LIBC_NAMESPACE::res_ninit(&state), 0);
  ASSERT_TRUE((state.options & RES_INIT) != 0);
  ASSERT_LE(state.ndots, 15u);
  ASSERT_LE(state.nscount, MAXNS);
  LIBC_NAMESPACE::res_nclose(&state);
}

TEST(LlvmLibcResNTest, TwoStatesAreIndependent) {
  struct __res_state first, second;
  LIBC_NAMESPACE::memset(&first, 0, sizeof(first));
  LIBC_NAMESPACE::memset(&second, 0, sizeof(second));
  ASSERT_EQ(LIBC_NAMESPACE::res_ninit(&first), 0);
  ASSERT_EQ(LIBC_NAMESPACE::res_ninit(&second), 0);
  first.ndots = 3;
  second.ndots = 5;
  ASSERT_EQ(first.ndots, 3u);
  ASSERT_EQ(second.ndots, 5u);
  LIBC_NAMESPACE::res_nclose(&first);
  LIBC_NAMESPACE::res_nclose(&second);
}

TEST(LlvmLibcResNTest, AQuestionIsWrittenOnTheWire) {
  struct __res_state state;
  LIBC_NAMESPACE::memset(&state, 0, sizeof(state));
  ASSERT_EQ(LIBC_NAMESPACE::res_ninit(&state), 0);
  unsigned char message[512];
  const int length = LIBC_NAMESPACE::res_nmkquery(
      &state, LIBC_NAMESPACE::resolv::OPCODE_QUERY, "example.com",
      LIBC_NAMESPACE::resolv::CLASS_IN, LIBC_NAMESPACE::resolv::TYPE_A, nullptr,
      0, nullptr, message, sizeof(message));
  ASSERT_EQ(length, 12 + 1 + 7 + 1 + 3 + 1 + 4);
  ASSERT_EQ((message[4] << 8) | message[5], 1);
  ASSERT_EQ(int(message[12]), 7);
  LIBC_NAMESPACE::res_nclose(&state);
}

TEST(LlvmLibcResNTest, NothingToAskAboutIsRefused) {
  struct __res_state state;
  LIBC_NAMESPACE::memset(&state, 0, sizeof(state));
  ASSERT_EQ(LIBC_NAMESPACE::res_ninit(&state), 0);
  unsigned char answer[512];
  ASSERT_EQ(LIBC_NAMESPACE::res_nquery(&state, nullptr,
                                       LIBC_NAMESPACE::resolv::CLASS_IN,
                                       LIBC_NAMESPACE::resolv::TYPE_A, answer,
                                       sizeof(answer)),
            -1);
  LIBC_NAMESPACE::res_nclose(&state);
}

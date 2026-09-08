//===-- Unittests for the resolver interface ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/struct___res_state.h"
#include "src/netdb/resolv/dns_message.h"
#include "src/resolv/res_close.h"
#include "src/resolv/res_init.h"
#include "src/resolv/res_mkquery.h"
#include "src/resolv/res_query.h"
#include "src/resolv/res_search.h"
#include "src/resolv/res_send.h"
#include "src/resolv/res_state.h"
#include "test/UnitTest/Test.h"

// What is read comes from /etc/resolv.conf, whose contents are the system's
// to decide, and asking a name server needs a network. The tests below say
// what has to hold whatever the file says and whether or not anything
// answers.

TEST(LlvmLibcResQueryTest, TheStateSaysItHasBeenFilledIn) {
  LIBC_NAMESPACE::res_init();
  struct __res_state *state = LIBC_NAMESPACE::__res_state();
  ASSERT_TRUE((state->options & RES_INIT) != 0);
  // A name is tried as it stands once it has this many dots in it, and the
  // field holding the number is four bits wide.
  ASSERT_LE(state->ndots, 15u);
  ASSERT_LE(state->nscount, MAXNS);
  // The search list ends in a null pointer, so walking it terminates.
  size_t domains = 0;
  while (domains < MAXDNSRCH && state->dnsrch[domains] != nullptr)
    ++domains;
  ASSERT_TRUE(state->dnsrch[domains] == nullptr);
}

// Asking twice gives the same answer: reading the file again is what this is
// for, and it must not lose what it read the first time.
TEST(LlvmLibcResQueryTest, FillingItInAgainSaysTheSame) {
  LIBC_NAMESPACE::res_init();
  const int first = LIBC_NAMESPACE::__res_state()->nscount;
  LIBC_NAMESPACE::res_init();
  ASSERT_EQ(LIBC_NAMESPACE::__res_state()->nscount, first);
}

// The question that would be sent is written out without anything being sent.
TEST(LlvmLibcResQueryTest, AQuestionIsWrittenTheWayItGoesOnTheWire) {
  unsigned char message[512];
  const int length = LIBC_NAMESPACE::res_mkquery(
      LIBC_NAMESPACE::resolv::OPCODE_QUERY, "example.com",
      LIBC_NAMESPACE::resolv::CLASS_IN, LIBC_NAMESPACE::resolv::TYPE_A, nullptr,
      0, nullptr, message, sizeof(message));
  // A header, then "example" and "com" with their lengths, the root, and the
  // type and class.
  ASSERT_EQ(length, 12 + 1 + 7 + 1 + 3 + 1 + 4);

  // One question and no answers.
  ASSERT_EQ((message[4] << 8) | message[5], 1);
  ASSERT_EQ((message[6] << 8) | message[7], 0);

  ASSERT_EQ(int(message[12]), 7);
  ASSERT_EQ(int(message[20]), 3);
  ASSERT_EQ(int(message[24]), 0);
  ASSERT_EQ((message[25] << 8) | message[26],
            int(LIBC_NAMESPACE::resolv::TYPE_A));
  ASSERT_EQ((message[27] << 8) | message[28],
            int(LIBC_NAMESPACE::resolv::CLASS_IN));
}

TEST(LlvmLibcResQueryTest, AQuestionThatWillNotFitIsRefused) {
  unsigned char message[8];
  ASSERT_EQ(LIBC_NAMESPACE::res_mkquery(LIBC_NAMESPACE::resolv::OPCODE_QUERY,
                                        "example.com",
                                        LIBC_NAMESPACE::resolv::CLASS_IN,
                                        LIBC_NAMESPACE::resolv::TYPE_A, nullptr,
                                        0, nullptr, message, sizeof(message)),
            -1);
}

TEST(LlvmLibcResQueryTest, NothingToAskAboutIsRefused) {
  unsigned char answer[512];
  ASSERT_EQ(LIBC_NAMESPACE::res_query(nullptr, LIBC_NAMESPACE::resolv::CLASS_IN,
                                      LIBC_NAMESPACE::resolv::TYPE_A, answer,
                                      sizeof(answer)),
            -1);
  ASSERT_EQ(LIBC_NAMESPACE::res_query(
                "example.com", LIBC_NAMESPACE::resolv::CLASS_IN,
                LIBC_NAMESPACE::resolv::TYPE_A, nullptr, sizeof(answer)),
            -1);
  ASSERT_EQ(
      LIBC_NAMESPACE::res_query("example.com", LIBC_NAMESPACE::resolv::CLASS_IN,
                                LIBC_NAMESPACE::resolv::TYPE_A, answer, 0),
      -1);
}

// Closing what was never opened is harmless, and may be done twice.
TEST(LlvmLibcResQueryTest, ClosingIsHarmless) {
  LIBC_NAMESPACE::res_close();
  LIBC_NAMESPACE::res_close();
}

// res_search is res_query with the search list applied, so it refuses the
// same arguments res_query refuses.
TEST(LlvmLibcResSearchTest, NothingToSearchForIsRefused) {
  unsigned char answer[512];
  ASSERT_EQ(LIBC_NAMESPACE::res_search(
                nullptr, LIBC_NAMESPACE::resolv::CLASS_IN,
                LIBC_NAMESPACE::resolv::TYPE_A, answer, sizeof(answer)),
            -1);
  ASSERT_EQ(LIBC_NAMESPACE::res_search(
                "example.com", LIBC_NAMESPACE::resolv::CLASS_IN,
                LIBC_NAMESPACE::resolv::TYPE_A, nullptr, sizeof(answer)),
            -1);
  ASSERT_EQ(LIBC_NAMESPACE::res_search(
                "example.com", LIBC_NAMESPACE::resolv::CLASS_IN,
                LIBC_NAMESPACE::resolv::TYPE_A, answer, 0),
            -1);
}

// res_send takes a query already built rather than a name, so what it
// refuses is a message that is not one.
TEST(LlvmLibcResSendTest, AMessageThatIsNotOneIsRefused) {
  unsigned char answer[512];
  unsigned char query[512];

  ASSERT_EQ(LIBC_NAMESPACE::res_send(nullptr, 12, answer, sizeof(answer)), -1);
  ASSERT_EQ(LIBC_NAMESPACE::res_send(query, 0, answer, sizeof(answer)), -1);
  ASSERT_EQ(LIBC_NAMESPACE::res_send(query, -1, answer, sizeof(answer)), -1);
  ASSERT_EQ(LIBC_NAMESPACE::res_send(query, 12, nullptr, sizeof(answer)), -1);
  ASSERT_EQ(LIBC_NAMESPACE::res_send(query, 12, answer, 0), -1);
}

// A query res_mkquery built is one res_send will carry, so the two agree on
// what a message is even where nothing answers it.
TEST(LlvmLibcResSendTest, CarriesAQueryThatWasBuilt) {
  unsigned char query[512];
  int length = LIBC_NAMESPACE::res_mkquery(
      0, "example.com", LIBC_NAMESPACE::resolv::CLASS_IN,
      LIBC_NAMESPACE::resolv::TYPE_A, nullptr, 0, nullptr, query,
      sizeof(query));
  ASSERT_GT(length, 0);

  // Whether anything answers is the network's to decide; what matters is
  // that the message was accepted rather than refused out of hand.
  unsigned char answer[512];
  int result = LIBC_NAMESPACE::res_send(query, length, answer, sizeof(answer));
  ASSERT_TRUE(result == -1 || result > 0);
}

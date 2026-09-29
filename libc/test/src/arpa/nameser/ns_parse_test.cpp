//===-- Unittests for the ns_ message parsing functions -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/types/ns_msg.h"
#include "hdr/types/ns_rr.h"
#include "src/arpa/nameser/ns_get16.h"
#include "src/arpa/nameser/ns_get32.h"
#include "src/arpa/nameser/ns_initparse.h"
#include "src/arpa/nameser/ns_name_uncompress.h"
#include "src/arpa/nameser/ns_parserr.h"
#include "src/arpa/nameser/ns_put16.h"
#include "src/arpa/nameser/ns_put32.h"
#include "src/arpa/nameser/ns_skiprr.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcNsParseTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {

constexpr int QUESTION = 0;
constexpr int ANSWER = 1;
constexpr int AUTHORITY = 2;

// A reply to "example.com A": one question and two answers, the first
// pointing back at the question's name, the second adding "www" to it.
constexpr unsigned char REPLY[] = {
    0x12, 0x34, 0x81, 0x80, 0, 1, 0, 2, 0, 0, 0, 0,
    // example.com A IN, at 12.
    7, 'e', 'x', 'a', 'm', 'p', 'l', 'e', 3, 'c', 'o', 'm', 0, 0, 1, 0, 1,
    // example.com A IN 300 1.2.3.4
    0xc0, 12, 0, 1, 0, 1, 0, 0, 0x01, 0x2c, 0, 4, 1, 2, 3, 4,
    // www.example.com A IN 60 5.6.7.8
    3, 'w', 'w', 'w', 0xc0, 12, 0, 1, 0, 1, 0, 0, 0, 60, 0, 4, 5, 6, 7, 8};

constexpr int QUESTION_LENGTH = 17;

bool same(const char *a, const char *b) {
  for (; *a != '\0' && *a == *b; ++a, ++b)
    ;
  return *a == *b;
}

} // anonymous namespace

TEST_F(LlvmLibcNsParseTest, ReadsTheHeader) {
  ns_msg msg;
  ASSERT_EQ(LIBC_NAMESPACE::ns_initparse(REPLY, sizeof(REPLY), &msg), 0);
  EXPECT_EQ(static_cast<int>(msg._id), 0x1234);
  EXPECT_EQ(static_cast<int>(msg._flags), 0x8180);
  EXPECT_EQ(static_cast<int>(msg._counts[QUESTION]), 1);
  EXPECT_EQ(static_cast<int>(msg._counts[ANSWER]), 2);
  EXPECT_EQ(static_cast<int>(msg._counts[AUTHORITY]), 0);
  EXPECT_EQ(msg._msg, REPLY);
  EXPECT_EQ(msg._eom, REPLY + sizeof(REPLY));
}

TEST_F(LlvmLibcNsParseTest, ReadsEachRecord) {
  ns_msg msg;
  ns_rr rr;
  ASSERT_EQ(LIBC_NAMESPACE::ns_initparse(REPLY, sizeof(REPLY), &msg), 0);

  ASSERT_EQ(LIBC_NAMESPACE::ns_parserr(&msg, QUESTION, 0, &rr), 0);
  EXPECT_TRUE(same(rr.name, "example.com"));
  EXPECT_EQ(static_cast<int>(rr.type), 1);
  EXPECT_EQ(static_cast<int>(rr.rr_class), 1);
  EXPECT_EQ(static_cast<int>(rr.rdlength), 0);
  EXPECT_EQ(rr.rdata, static_cast<const unsigned char *>(nullptr));

  // Out of order, which has to go back to the start of the section.
  ASSERT_EQ(LIBC_NAMESPACE::ns_parserr(&msg, ANSWER, 1, &rr), 0);
  EXPECT_TRUE(same(rr.name, "www.example.com"));
  EXPECT_EQ(rr.ttl, 60u);
  ASSERT_EQ(static_cast<int>(rr.rdlength), 4);
  EXPECT_EQ(static_cast<int>(rr.rdata[0]), 5);

  ASSERT_EQ(LIBC_NAMESPACE::ns_parserr(&msg, ANSWER, 0, &rr), 0);
  EXPECT_TRUE(same(rr.name, "example.com"));
  EXPECT_EQ(rr.ttl, 300u);
  ASSERT_EQ(static_cast<int>(rr.rdlength), 4);
  EXPECT_EQ(static_cast<int>(rr.rdata[3]), 4);

  // -1 is whichever comes next.
  ASSERT_EQ(LIBC_NAMESPACE::ns_parserr(&msg, ANSWER, -1, &rr), 0);
  EXPECT_TRUE(same(rr.name, "www.example.com"));
}

TEST_F(LlvmLibcNsParseTest, RefusesARecordThatIsNotThere) {
  ns_msg msg;
  ns_rr rr;
  ASSERT_EQ(LIBC_NAMESPACE::ns_initparse(REPLY, sizeof(REPLY), &msg), 0);
  EXPECT_EQ(LIBC_NAMESPACE::ns_parserr(&msg, ANSWER, 2, &rr), -1);
  ASSERT_ERRNO_EQ(ENODEV);
  EXPECT_EQ(LIBC_NAMESPACE::ns_parserr(&msg, AUTHORITY, 0, &rr), -1);
  ASSERT_ERRNO_EQ(ENODEV);
  EXPECT_EQ(LIBC_NAMESPACE::ns_parserr(&msg, 4, 0, &rr), -1);
  ASSERT_ERRNO_EQ(ENODEV);
}

TEST_F(LlvmLibcNsParseTest, RefusesAMessageOfTheWrongLength) {
  ns_msg msg;
  EXPECT_EQ(LIBC_NAMESPACE::ns_initparse(REPLY, sizeof(REPLY) - 1, &msg), -1);
  ASSERT_ERRNO_EQ(EMSGSIZE);

  unsigned char longer[sizeof(REPLY) + 1] = {};
  for (size_t i = 0; i < sizeof(REPLY); ++i)
    longer[i] = REPLY[i];
  EXPECT_EQ(LIBC_NAMESPACE::ns_initparse(longer, sizeof(longer), &msg), -1);
  ASSERT_ERRNO_EQ(EMSGSIZE);

  EXPECT_EQ(LIBC_NAMESPACE::ns_initparse(REPLY, 11, &msg), -1);
  ASSERT_ERRNO_EQ(EMSGSIZE);
}

TEST_F(LlvmLibcNsParseTest, SkipsRecords) {
  const unsigned char *eom = REPLY + sizeof(REPLY);
  EXPECT_EQ(LIBC_NAMESPACE::ns_skiprr(REPLY + 12, eom, QUESTION, 1),
            QUESTION_LENGTH);
  EXPECT_EQ(
      LIBC_NAMESPACE::ns_skiprr(REPLY + 12 + QUESTION_LENGTH, eom, ANSWER, 2),
      static_cast<int>(sizeof(REPLY)) - 12 - QUESTION_LENGTH);
  EXPECT_EQ(
      LIBC_NAMESPACE::ns_skiprr(REPLY + 12 + QUESTION_LENGTH, eom, ANSWER, 3),
      -1);
  ASSERT_ERRNO_EQ(EMSGSIZE);
}

TEST_F(LlvmLibcNsParseTest, UncompressesAName) {
  char name[64];
  const unsigned char *eom = REPLY + sizeof(REPLY);
  const unsigned char *www = REPLY + sizeof(REPLY) - 20;
  EXPECT_EQ(
      LIBC_NAMESPACE::ns_name_uncompress(REPLY, eom, www, name, sizeof(name)),
      6);
  EXPECT_TRUE(same(name, "www.example.com"));
  EXPECT_EQ(LIBC_NAMESPACE::ns_name_uncompress(REPLY, eom, www, name, 8), -1);
  ASSERT_ERRNO_EQ(EMSGSIZE);
}

TEST_F(LlvmLibcNsParseTest, ReadsAndWritesNetworkOrder) {
  unsigned char buf[4];
  LIBC_NAMESPACE::ns_put16(0xbeef, buf);
  EXPECT_EQ(static_cast<int>(buf[0]), 0xbe);
  EXPECT_EQ(LIBC_NAMESPACE::ns_get16(buf), 0xbeefu);
  LIBC_NAMESPACE::ns_put32(0xdeadbeef, buf);
  EXPECT_EQ(static_cast<int>(buf[0]), 0xde);
  EXPECT_EQ(static_cast<int>(buf[3]), 0xef);
  EXPECT_EQ(LIBC_NAMESPACE::ns_get32(buf), 0xdeadbeeful);
}

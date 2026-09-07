//===-- Unittests for building and reading a name server's messages -------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/resolv/dns_message.h"
#include "src/string/strcmp.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::resolv::build_query;
using LIBC_NAMESPACE::resolv::CLASS_IN;
using LIBC_NAMESPACE::resolv::for_each_answer;
using LIBC_NAMESPACE::resolv::HEADER_SIZE;
using LIBC_NAMESPACE::resolv::MAX_MESSAGE;
using LIBC_NAMESPACE::resolv::read_name;
using LIBC_NAMESPACE::resolv::Record;
using LIBC_NAMESPACE::resolv::skip_name;
using LIBC_NAMESPACE::resolv::Status;
using LIBC_NAMESPACE::resolv::TYPE_A;
using LIBC_NAMESPACE::resolv::TYPE_AAAA;
using LIBC_NAMESPACE::resolv::TYPE_CNAME;

TEST(LlvmLibcDnsMessageTest, QueryHasTheNameAsLabels) {
  unsigned char message[MAX_MESSAGE];
  const size_t length = build_query("www.example.com", TYPE_A, CLASS_IN, 0x1234,
                                    message, sizeof(message));
  // A header, then 3www7example3com0, then the type and the class.
  ASSERT_EQ(length, HEADER_SIZE + 17 + 4);
  EXPECT_EQ(message[0], static_cast<unsigned char>(0x12));
  EXPECT_EQ(message[1], static_cast<unsigned char>(0x34));
  // One question, and a request that the server ask others on our behalf.
  EXPECT_EQ(message[2], static_cast<unsigned char>(0x01));
  EXPECT_EQ(message[5], static_cast<unsigned char>(0x01));

  EXPECT_EQ(message[HEADER_SIZE], static_cast<unsigned char>(3));
  EXPECT_EQ(message[HEADER_SIZE + 1], static_cast<unsigned char>('w'));
  EXPECT_EQ(message[HEADER_SIZE + 4], static_cast<unsigned char>(7));
  EXPECT_EQ(message[HEADER_SIZE + 12], static_cast<unsigned char>(3));
  EXPECT_EQ(message[HEADER_SIZE + 16], static_cast<unsigned char>(0));
  // The type and the class follow it, each two bytes with the high one
  // first.
  EXPECT_EQ(message[HEADER_SIZE + 17], static_cast<unsigned char>(0));
  EXPECT_EQ(message[HEADER_SIZE + 18], static_cast<unsigned char>(TYPE_A));
  EXPECT_EQ(message[HEADER_SIZE + 19], static_cast<unsigned char>(0));
  EXPECT_EQ(message[HEADER_SIZE + 20], static_cast<unsigned char>(1));
}

TEST(LlvmLibcDnsMessageTest, RefusesWhatIsNotAName) {
  unsigned char message[MAX_MESSAGE];
  EXPECT_EQ(build_query("", TYPE_A, CLASS_IN, 1, message, sizeof(message)),
            size_t(0));
  EXPECT_EQ(build_query("a..b", TYPE_A, CLASS_IN, 1, message, sizeof(message)),
            size_t(0));
  EXPECT_EQ(build_query(nullptr, TYPE_A, CLASS_IN, 1, message, sizeof(message)),
            size_t(0));
  // A part of a name is at most sixty three bytes.
  char too_long[80];
  for (size_t i = 0; i < 70; ++i)
    too_long[i] = 'a';
  too_long[70] = '\0';
  EXPECT_EQ(
      build_query(too_long, TYPE_A, CLASS_IN, 1, message, sizeof(message)),
      size_t(0));
  // And there has to be room to put it.
  unsigned char tiny[8];
  EXPECT_EQ(build_query("a.b", TYPE_A, CLASS_IN, 1, tiny, sizeof(tiny)),
            size_t(0));
}

TEST(LlvmLibcDnsMessageTest, ANameThatEndsInADotIsTheSameName) {
  unsigned char with_dot[MAX_MESSAGE];
  unsigned char without[MAX_MESSAGE];
  const size_t a = build_query("example.com.", TYPE_A, CLASS_IN, 1, with_dot,
                               sizeof(with_dot));
  const size_t b =
      build_query("example.com", TYPE_A, CLASS_IN, 1, without, sizeof(without));
  ASSERT_GT(a, size_t(0));
  ASSERT_EQ(a, b);
  for (size_t i = 0; i < a; ++i)
    ASSERT_EQ(with_dot[i], without[i]);
}

namespace {

// A message holding one question and the answers that follow it.
struct Builder {
  unsigned char bytes[MAX_MESSAGE] = {};
  size_t length = 0;

  void start(uint16_t id, size_t answers, unsigned char code = 0) {
    length =
        build_query("example.com", TYPE_A, CLASS_IN, id, bytes, sizeof(bytes));
    // Say it is an answer, and how many there are.
    bytes[2] = 0x80;
    bytes[3] = code;
    bytes[6] = static_cast<unsigned char>(answers >> 8);
    bytes[7] = static_cast<unsigned char>(answers);
  }

  // Adds a record whose name points back at the question's.
  void answer(uint16_t type, const unsigned char *data, size_t size) {
    bytes[length++] = 0xC0;
    bytes[length++] = static_cast<unsigned char>(HEADER_SIZE);
    bytes[length++] = static_cast<unsigned char>(type >> 8);
    bytes[length++] = static_cast<unsigned char>(type);
    bytes[length++] = 0;
    bytes[length++] = 1; // The internet class.
    for (int i = 0; i < 4; ++i)
      bytes[length++] = 0; // How long it may be kept.
    bytes[length++] = static_cast<unsigned char>(size >> 8);
    bytes[length++] = static_cast<unsigned char>(size);
    for (size_t i = 0; i < size; ++i)
      bytes[length++] = data[i];
  }
};

} // anonymous namespace

TEST(LlvmLibcDnsMessageTest, ReadsTheAnswers) {
  const unsigned char first[4] = {93, 184, 216, 34};
  const unsigned char second[4] = {93, 184, 216, 35};
  Builder message;
  message.start(0x4321, 2);
  message.answer(TYPE_A, first, 4);
  message.answer(TYPE_A, second, 4);

  int seen = 0;
  unsigned char collected[2][4] = {};
  const Status status = for_each_answer(
      message.bytes, message.length, 0x4321, [&](const Record &record) {
        if (seen < 2 && record.type == TYPE_A && record.length == 4)
          for (size_t i = 0; i < 4; ++i)
            collected[seen][i] = record.data[i];
        ++seen;
      });
  EXPECT_EQ(static_cast<int>(status), static_cast<int>(Status::Ok));
  ASSERT_EQ(seen, 2);
  EXPECT_EQ(collected[0][3], static_cast<unsigned char>(34));
  EXPECT_EQ(collected[1][3], static_cast<unsigned char>(35));
}

TEST(LlvmLibcDnsMessageTest, AnAnswerToSomeoneElseIsNotAnAnswer) {
  const unsigned char address[4] = {1, 2, 3, 4};
  Builder message;
  message.start(0x1111, 1);
  message.answer(TYPE_A, address, 4);

  int seen = 0;
  const Status status = for_each_answer(message.bytes, message.length, 0x2222,
                                        [&](const Record &) { ++seen; });
  EXPECT_EQ(static_cast<int>(status), static_cast<int>(Status::Failed));
  EXPECT_EQ(seen, 0);
}

TEST(LlvmLibcDnsMessageTest, TellsAMissingNameFromAServerThatWentWrong) {
  Builder missing;
  missing.start(1, 0, 3); // The name does not exist.
  EXPECT_EQ(static_cast<int>(for_each_answer(missing.bytes, missing.length, 1,
                                             [](const Record &) {})),
            static_cast<int>(Status::NoName));

  Builder failed;
  failed.start(1, 0, 2); // Something went wrong at the server.
  EXPECT_EQ(static_cast<int>(for_each_answer(failed.bytes, failed.length, 1,
                                             [](const Record &) {})),
            static_cast<int>(Status::Failed));
}

TEST(LlvmLibcDnsMessageTest, ATruncatedAnswerIsNoAnswer) {
  const unsigned char address[4] = {1, 2, 3, 4};
  Builder message;
  message.start(1, 1);
  message.answer(TYPE_A, address, 4);
  message.bytes[2] |= 0x02; // What is missing may be what was wanted.
  EXPECT_EQ(static_cast<int>(for_each_answer(message.bytes, message.length, 1,
                                             [](const Record &) {})),
            static_cast<int>(Status::Failed));
}

TEST(LlvmLibcDnsMessageTest, ReadsANameBackFromItsLabels) {
  unsigned char message[MAX_MESSAGE];
  const size_t length = build_query("www.example.com", TYPE_AAAA, CLASS_IN, 1,
                                    message, sizeof(message));
  char name[256];
  const size_t taken =
      read_name(message, length, HEADER_SIZE, name, sizeof(name));
  EXPECT_EQ(taken, size_t(17));
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(name, "www.example.com"), 0);
  EXPECT_EQ(skip_name(message, length, HEADER_SIZE), HEADER_SIZE + 17);
}

TEST(LlvmLibcDnsMessageTest, FollowsAPointerToANameAlreadyThere) {
  unsigned char message[MAX_MESSAGE];
  size_t length =
      build_query("example.com", TYPE_A, CLASS_IN, 1, message, sizeof(message));
  // A name that is just a pointer back to the question's.
  const size_t pointer_at = length;
  message[length++] = 0xC0;
  message[length++] = static_cast<unsigned char>(HEADER_SIZE);

  char name[256];
  EXPECT_EQ(read_name(message, length, pointer_at, name, sizeof(name)),
            size_t(2));
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(name, "example.com"), 0);
}

TEST(LlvmLibcDnsMessageTest, RefusesANameThatWouldGoRoundForEver) {
  unsigned char message[64] = {};
  // A pointer to itself, which a message is not allowed to hold.
  message[20] = 0xC0;
  message[21] = 20;
  char name[256];
  EXPECT_EQ(read_name(message, sizeof(message), 20, name, sizeof(name)),
            size_t(0));

  // And one that points forwards, which would let a chain be built.
  message[20] = 0xC0;
  message[21] = 30;
  EXPECT_EQ(read_name(message, sizeof(message), 20, name, sizeof(name)),
            size_t(0));
}

TEST(LlvmLibcDnsMessageTest, RefusesALabelThatRunsPastTheMessage) {
  unsigned char message[32] = {};
  message[10] = 40; // Longer than what is left.
  char name[256];
  EXPECT_EQ(read_name(message, sizeof(message), 10, name, sizeof(name)),
            size_t(0));
  EXPECT_EQ(skip_name(message, sizeof(message), 10), size_t(0));
}

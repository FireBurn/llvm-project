//===-- Unittests for dn_expand and dn_comp -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/resolv/dn_comp.h"
#include "src/resolv/dn_expand.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "test/UnitTest/Test.h"

namespace {

// Builds "www.example.com" followed by a pointer back to the "example.com"
// part of it, which is how a message avoids writing a name twice.
size_t build_message(unsigned char *msg, size_t &example_at,
                     size_t &pointer_at) {
  size_t n = 0;
  msg[n++] = 3;
  LIBC_NAMESPACE::inline_memcpy(msg + n, "www", 3);
  n += 3;
  example_at = n;
  msg[n++] = 7;
  LIBC_NAMESPACE::inline_memcpy(msg + n, "example", 7);
  n += 7;
  msg[n++] = 3;
  LIBC_NAMESPACE::inline_memcpy(msg + n, "com", 3);
  n += 3;
  msg[n++] = 0;
  pointer_at = n;
  msg[n++] = 0xc0;
  msg[n++] = static_cast<unsigned char>(example_at);
  return n;
}

} // anonymous namespace

TEST(LlvmLibcDnExpandTest, ReadsAName) {
  unsigned char msg[64] = {};
  size_t example_at = 0, pointer_at = 0;
  const size_t len = build_message(msg, example_at, pointer_at);

  char out[256];
  const int used =
      LIBC_NAMESPACE::dn_expand(msg, msg + len, msg, out, sizeof(out));
  ASSERT_EQ(used, 17);
  ASSERT_STREQ(out, "www.example.com");
}

// Where the name is only a pointer, two bytes of the message were used.
TEST(LlvmLibcDnExpandTest, FollowsAPointer) {
  unsigned char msg[64] = {};
  size_t example_at = 0, pointer_at = 0;
  const size_t len = build_message(msg, example_at, pointer_at);

  char out[256];
  const int used = LIBC_NAMESPACE::dn_expand(msg, msg + len, msg + pointer_at,
                                             out, sizeof(out));
  ASSERT_EQ(used, 2);
  ASSERT_STREQ(out, "example.com");
}

TEST(LlvmLibcDnExpandTest, RefusesWhatWillNotFit) {
  unsigned char msg[64] = {};
  size_t example_at = 0, pointer_at = 0;
  const size_t len = build_message(msg, example_at, pointer_at);

  char small[4];
  ASSERT_EQ(
      LIBC_NAMESPACE::dn_expand(msg, msg + len, msg, small, sizeof(small)), -1);
}

// A pointer that leads forward, or to itself, would never end.
TEST(LlvmLibcDnExpandTest, RefusesAPointerThatDoesNotLeadBack) {
  unsigned char msg[8] = {};
  msg[0] = 0xc0;
  msg[1] = 0; // Points at itself.
  char out[64];
  ASSERT_EQ(LIBC_NAMESPACE::dn_expand(msg, msg + 2, msg, out, sizeof(out)), -1);
}

TEST(LlvmLibcDnExpandTest, WritesAName) {
  unsigned char packed[64];
  const int len = LIBC_NAMESPACE::dn_comp("a.example.com", packed,
                                          sizeof(packed), nullptr, nullptr);
  ASSERT_EQ(len, 15);
  ASSERT_EQ(int(packed[0]), 1);
  ASSERT_EQ(char(packed[1]), 'a');
  ASSERT_EQ(int(packed[2]), 7);
  ASSERT_EQ(int(packed[len - 1]), 0);
}

// What was written is what reading it back gives.
TEST(LlvmLibcDnExpandTest, WhatIsWrittenReadsBack) {
  unsigned char packed[64];
  const int len = LIBC_NAMESPACE::dn_comp("one.two.example", packed,
                                          sizeof(packed), nullptr, nullptr);
  ASSERT_GT(len, 0);
  char out[256];
  ASSERT_EQ(
      LIBC_NAMESPACE::dn_expand(packed, packed + len, packed, out, sizeof(out)),
      len);
  ASSERT_STREQ(out, "one.two.example");
}

TEST(LlvmLibcDnExpandTest, RefusesWhatWillNotFitWhenWriting) {
  unsigned char tiny[4];
  ASSERT_EQ(LIBC_NAMESPACE::dn_comp("a.example.com", tiny, sizeof(tiny),
                                    nullptr, nullptr),
            -1);
}

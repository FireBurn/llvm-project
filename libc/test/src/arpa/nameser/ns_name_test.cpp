//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the ns_name_ functions and the name helpers beside them
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/types/ns_msg.h"
#include "hdr/types/ns_rr.h"
#include "src/arpa/nameser/ns_makecanon.h"
#include "src/arpa/nameser/ns_msg_getflag.h"
#include "src/arpa/nameser/ns_name_compress.h"
#include "src/arpa/nameser/ns_name_ntop.h"
#include "src/arpa/nameser/ns_name_pack.h"
#include "src/arpa/nameser/ns_name_pton.h"
#include "src/arpa/nameser/ns_name_rollback.h"
#include "src/arpa/nameser/ns_name_skip.h"
#include "src/arpa/nameser/ns_name_unpack.h"
#include "src/arpa/nameser/ns_samename.h"
#include "src/arpa/nameser/ns_sprintrr.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcNsNameTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {

bool same(const char *a, const char *b) {
  for (; *a != '\0' && *a == *b; ++a, ++b) {
  }
  return *a == *b;
}

constexpr unsigned char EXAMPLE_COM[] = {7,   'e', 'x', 'a', 'm', 'p', 'l',
                                         'e', 3,   'c', 'o', 'm', 0};

} // namespace

TEST_F(LlvmLibcNsNameTest, NtopWritesLabelsWithDots) {
  char out[64];
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_ntop(EXAMPLE_COM, out, sizeof(out)), 12);
  ASSERT_TRUE(same(out, "example.com"));
}

TEST_F(LlvmLibcNsNameTest, NtopWritesTheRootAsADot) {
  const unsigned char root[] = {0};
  char out[4];
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_ntop(root, out, sizeof(out)), 2);
  ASSERT_TRUE(same(out, "."));
}

TEST_F(LlvmLibcNsNameTest, NtopEscapesDotsAndUnprintableBytes) {
  const unsigned char name[] = {4, 'a', '.', 'b', 1, 2, 'x', 0x7f, 0};
  char out[32];
  ASSERT_GT(LIBC_NAMESPACE::ns_name_ntop(name, out, sizeof(out)), 0);
  ASSERT_TRUE(same(out, "a\\.b\\001.x\\127"));
}

TEST_F(LlvmLibcNsNameTest, NtopFailsWhenItDoesNotFit) {
  char out[11];
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_ntop(EXAMPLE_COM, out, sizeof(out)), -1);
  ASSERT_ERRNO_EQ(EMSGSIZE);
}

TEST_F(LlvmLibcNsNameTest, PtonReadsAbsoluteAndRelativeNames) {
  unsigned char out[64];
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_pton("example.com.", out, sizeof(out)), 1);
  for (unsigned i = 0; i < sizeof(EXAMPLE_COM); ++i)
    ASSERT_EQ(static_cast<int>(out[i]), static_cast<int>(EXAMPLE_COM[i]));
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_pton("example.com", out, sizeof(out)), 0);
  for (unsigned i = 0; i < sizeof(EXAMPLE_COM); ++i)
    ASSERT_EQ(static_cast<int>(out[i]), static_cast<int>(EXAMPLE_COM[i]));
}

TEST_F(LlvmLibcNsNameTest, PtonReadsTheRootAndEscapes) {
  unsigned char out[64];
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_pton(".", out, sizeof(out)), 1);
  ASSERT_EQ(static_cast<int>(out[0]), 0);
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_pton("a\\.b\\001", out, sizeof(out)), 0);
  ASSERT_EQ(static_cast<int>(out[0]), 4);
  ASSERT_EQ(static_cast<int>(out[2]), 46);
  ASSERT_EQ(static_cast<int>(out[4]), 1);
  ASSERT_EQ(static_cast<int>(out[5]), 0);
}

TEST_F(LlvmLibcNsNameTest, PtonRejectsBadNames) {
  unsigned char out[300];
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_pton("a..b", out, sizeof(out)), -1);
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_pton(".a", out, sizeof(out)), -1);
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_pton("a\\", out, sizeof(out)), -1);
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_pton("a\\999", out, sizeof(out)), -1);
  char label[70];
  for (int i = 0; i < 64; ++i)
    label[i] = 'a';
  label[64] = '\0';
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_pton(label, out, sizeof(out)), -1);
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_pton("example.com", out, 5), -1);
  ASSERT_ERRNO_EQ(EMSGSIZE);
}

TEST_F(LlvmLibcNsNameTest, UnpackFollowsPointers) {
  // example.com at 0, then www followed by a pointer to it.
  const unsigned char msg[] = {7,   'e', 'x', 'a', 'm', 'p', 'l', 'e',  3, 'c',
                               'o', 'm', 0,   3,   'w', 'w', 'w', 0xc0, 0};
  unsigned char out[64];
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_unpack(msg, msg + sizeof(msg), msg + 13,
                                           out, sizeof(out)),
            6);
  ASSERT_EQ(static_cast<int>(out[0]), 3);
  ASSERT_EQ(static_cast<int>(out[4]), 7);
  ASSERT_EQ(static_cast<int>(out[12]), 3);
  ASSERT_EQ(static_cast<int>(out[13]), 99);
}

TEST_F(LlvmLibcNsNameTest, UnpackRejectsLoopsAndTruncation) {
  const unsigned char loop[] = {0xc0, 0};
  unsigned char out[64];
  ASSERT_EQ(
      LIBC_NAMESPACE::ns_name_unpack(loop, loop + 2, loop, out, sizeof(out)),
      -1);
  ASSERT_ERRNO_EQ(EMSGSIZE);
  const unsigned char cut[] = {5, 'a', 'b'};
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_unpack(cut, cut + 3, cut, out, sizeof(out)),
            -1);
  ASSERT_ERRNO_EQ(EMSGSIZE);
}

TEST_F(LlvmLibcNsNameTest, SkipStepsOverPlainAndCompressedNames) {
  const unsigned char msg[] = {3, 'w', 'w', 'w', 0xc0, 0, 9};
  const unsigned char *p = msg;
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_skip(&p, msg + sizeof(msg)), 0);
  ASSERT_EQ(static_cast<int>(p - msg), 6);
  const unsigned char plain[] = {1, 'a', 0, 7};
  p = plain;
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_skip(&p, plain + sizeof(plain)), 0);
  ASSERT_EQ(static_cast<int>(p - plain), 3);
  const unsigned char cut[] = {4, 'a'};
  p = cut;
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_skip(&p, cut + 2), -1);
  ASSERT_ERRNO_EQ(EMSGSIZE);
}

TEST_F(LlvmLibcNsNameTest, PackWithoutPointersCopiesTheName) {
  unsigned char out[64];
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_pack(EXAMPLE_COM, out, sizeof(out), nullptr,
                                         nullptr),
            13);
  ASSERT_EQ(static_cast<int>(out[0]), 7);
  ASSERT_EQ(
      LIBC_NAMESPACE::ns_name_pack(EXAMPLE_COM, out, 12, nullptr, nullptr), -1);
  ASSERT_ERRNO_EQ(EMSGSIZE);
}

TEST_F(LlvmLibcNsNameTest, CompressPointsAtAnEarlierName) {
  unsigned char msg[128];
  const unsigned char *ptrs[8] = {msg, nullptr};
  int n = LIBC_NAMESPACE::ns_name_compress("example.com", msg, sizeof(msg),
                                           ptrs, ptrs + 8);
  ASSERT_EQ(n, 13);
  n = LIBC_NAMESPACE::ns_name_compress("www.EXAMPLE.com", msg + 13,
                                       sizeof(msg) - 13, ptrs, ptrs + 8);
  ASSERT_EQ(n, 6);
  ASSERT_EQ(static_cast<int>(msg[13]), 3);
  ASSERT_EQ(static_cast<int>(msg[17]), 0xc0);
  ASSERT_EQ(static_cast<int>(msg[18]), 0);
  // The whole of a name seen before is one pointer.
  n = LIBC_NAMESPACE::ns_name_compress("example.com", msg + 19,
                                       sizeof(msg) - 19, ptrs, ptrs + 8);
  ASSERT_EQ(n, 2);
  ASSERT_EQ(static_cast<int>(msg[19]), 0xc0);
  ASSERT_EQ(static_cast<int>(msg[20]), 0);
  // And what it unpacks to is what went in.
  unsigned char out[64];
  ASSERT_EQ(
      LIBC_NAMESPACE::ns_name_unpack(msg, msg + 21, msg + 13, out, sizeof(out)),
      6);
  ASSERT_EQ(static_cast<int>(out[12]), 3);
}

TEST_F(LlvmLibcNsNameTest, RollbackForgetsNamesFromAPoint) {
  unsigned char msg[128];
  const unsigned char *ptrs[8] = {msg, nullptr};
  LIBC_NAMESPACE::ns_name_compress("example.com", msg, sizeof(msg), ptrs,
                                   ptrs + 8);
  LIBC_NAMESPACE::ns_name_compress("other.org", msg + 13, sizeof(msg) - 13,
                                   ptrs, ptrs + 8);
  LIBC_NAMESPACE::ns_name_rollback(msg + 13, ptrs, ptrs + 8);
  // Nothing at 13 or after is a target now, so this one is written out.
  ASSERT_EQ(LIBC_NAMESPACE::ns_name_compress("www.other.org", msg + 13,
                                             sizeof(msg) - 13, ptrs, ptrs + 8),
            15);
}

TEST_F(LlvmLibcNsNameTest, SameNameIgnoresCaseAndTheFinalDot) {
  ASSERT_EQ(LIBC_NAMESPACE::ns_samename("example.com", "EXAMPLE.com."), 1);
  ASSERT_EQ(LIBC_NAMESPACE::ns_samename("example.com", "example.org"), 0);
  ASSERT_EQ(LIBC_NAMESPACE::ns_samename("example.com", "example.com.x"), 0);
  ASSERT_EQ(LIBC_NAMESPACE::ns_samename(".", ""), 1);
}

TEST_F(LlvmLibcNsNameTest, MakeCanonAddsOneDot) {
  char out[16];
  ASSERT_EQ(LIBC_NAMESPACE::ns_makecanon("a.b", out, sizeof(out)), 0);
  ASSERT_TRUE(same(out, "a.b."));
  ASSERT_EQ(LIBC_NAMESPACE::ns_makecanon("a.b.", out, sizeof(out)), 0);
  ASSERT_TRUE(same(out, "a.b."));
  ASSERT_EQ(LIBC_NAMESPACE::ns_makecanon("a\\.", out, sizeof(out)), 0);
  ASSERT_TRUE(same(out, "a\\.."));
  ASSERT_EQ(LIBC_NAMESPACE::ns_makecanon("a.b", out, 4), -1);
  ASSERT_ERRNO_EQ(EMSGSIZE);
}

TEST_F(LlvmLibcNsNameTest, GetFlagReadsTheHeaderFlags) {
  ns_msg handle = {};
  handle._flags = 0x8180 | 0x0005; // Response, RD, RA, rcode 5.
  ASSERT_EQ(LIBC_NAMESPACE::ns_msg_getflag(handle, 0), 1);
  ASSERT_EQ(LIBC_NAMESPACE::ns_msg_getflag(handle, 1), 0);
  ASSERT_EQ(LIBC_NAMESPACE::ns_msg_getflag(handle, 4), 1);
  ASSERT_EQ(LIBC_NAMESPACE::ns_msg_getflag(handle, 5), 1);
  ASSERT_EQ(LIBC_NAMESPACE::ns_msg_getflag(handle, 9), 5);
  ASSERT_EQ(LIBC_NAMESPACE::ns_msg_getflag(handle, 10), -1);
  ASSERT_ERRNO_EQ(EINVAL);
}

TEST_F(LlvmLibcNsNameTest, SprintrrWritesOneLine) {
  ns_msg handle = {};
  ns_rr rr = {};
  const unsigned char data[] = {1, 2, 3, 4};
  const char name[] = "example.com";
  for (unsigned i = 0; i < sizeof(name); ++i)
    rr.name[i] = name[i];
  rr.type = 1;
  rr.rr_class = 1;
  rr.ttl = 300;
  rr.rdlength = 4;
  rr.rdata = data;
  char out[128];
  ASSERT_EQ(LIBC_NAMESPACE::ns_sprintrr(&handle, &rr, nullptr, nullptr, out,
                                        sizeof(out)),
            41);
  ASSERT_TRUE(same(out, "example.com\t300\tIN\tTYPE1\t\\# 4 01 02 03 04"));
  char small[8];
  ASSERT_EQ(LIBC_NAMESPACE::ns_sprintrr(&handle, &rr, nullptr, nullptr, small,
                                        sizeof(small)),
            -1);
  ASSERT_ERRNO_EQ(ENOSPC);
}

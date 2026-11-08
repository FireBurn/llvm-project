//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
///
/// \file
/// Unittests for b64_ntop and b64_pton
///
//===----------------------------------------------------------------------===//

#include "src/resolv/b64_ntop.h"
#include "src/resolv/b64_pton.h"
#include "src/string/memory_utils/inline_memcmp.h"
#include "test/UnitTest/Test.h"

namespace {

template <size_t N>
void encodes(const char *plain, size_t length, const char (&expected)[N]) {
  char out[64];
  const int n = LIBC_NAMESPACE::b64_ntop(
      reinterpret_cast<const unsigned char *>(plain), length, out, sizeof(out));
  ASSERT_EQ(size_t(n), size_t(N - 1));
  EXPECT_STREQ(out, expected);
}

} // anonymous namespace

TEST(LlvmLibcB64Test, NtopMatchesTheRfc4648Vectors) {
  encodes("", 0, "");
  encodes("f", 1, "Zg==");
  encodes("fo", 2, "Zm8=");
  encodes("foo", 3, "Zm9v");
  encodes("foob", 4, "Zm9vYg==");
  encodes("fooba", 5, "Zm9vYmE=");
  encodes("foobar", 6, "Zm9vYmFy");
}

TEST(LlvmLibcB64Test, NtopNeedsRoomForTheTerminator) {
  char out[8];
  const unsigned char data[3] = {'f', 'o', 'o'};
  EXPECT_EQ(LIBC_NAMESPACE::b64_ntop(data, 3, out, 4), -1);
  EXPECT_EQ(LIBC_NAMESPACE::b64_ntop(data, 3, out, 5), 4);
  EXPECT_EQ(LIBC_NAMESPACE::b64_ntop(data, 1, out, 4), -1);
  EXPECT_EQ(LIBC_NAMESPACE::b64_ntop(data, 1, out, 5), 4);
}

TEST(LlvmLibcB64Test, NtopUsesAllSixtyFourDigits) {
  const unsigned char data[3] = {0xfb, 0xff, 0xbf};
  char out[8];
  ASSERT_EQ(LIBC_NAMESPACE::b64_ntop(data, 3, out, sizeof(out)), 4);
  EXPECT_STREQ(out, "+/+/");
}

TEST(LlvmLibcB64Test, PtonMatchesTheRfc4648Vectors) {
  unsigned char out[16];
  ASSERT_EQ(LIBC_NAMESPACE::b64_pton("Zm9vYmE=", out, sizeof(out)), 5);
  EXPECT_EQ(LIBC_NAMESPACE::inline_memcmp(out, "fooba", 5), 0);
  ASSERT_EQ(LIBC_NAMESPACE::b64_pton("Zg==", out, sizeof(out)), 1);
  EXPECT_EQ(out[0], static_cast<unsigned char>('f'));
  ASSERT_EQ(LIBC_NAMESPACE::b64_pton("Zm9vYmFy", out, sizeof(out)), 6);
  EXPECT_EQ(LIBC_NAMESPACE::inline_memcmp(out, "foobar", 6), 0);
  EXPECT_EQ(LIBC_NAMESPACE::b64_pton("", out, sizeof(out)), 0);
}

TEST(LlvmLibcB64Test, PtonSkipsWhiteSpace) {
  unsigned char out[16];
  ASSERT_EQ(LIBC_NAMESPACE::b64_pton(" Zm9v\n YmE =\t", out, sizeof(out)), 5);
  EXPECT_EQ(LIBC_NAMESPACE::inline_memcmp(out, "fooba", 5), 0);
  ASSERT_EQ(LIBC_NAMESPACE::b64_pton("Zg = =", out, sizeof(out)), 1);
}

TEST(LlvmLibcB64Test, PtonRefusesWhatIsNotBase64) {
  unsigned char out[16];
  EXPECT_EQ(LIBC_NAMESPACE::b64_pton("Zm9v!", out, sizeof(out)), -1);
  EXPECT_EQ(LIBC_NAMESPACE::b64_pton("Z", out, sizeof(out)), -1);
  EXPECT_EQ(LIBC_NAMESPACE::b64_pton("Zm9vY", out, sizeof(out)), -1);
  EXPECT_EQ(LIBC_NAMESPACE::b64_pton("=", out, sizeof(out)), -1);
  EXPECT_EQ(LIBC_NAMESPACE::b64_pton("Zg=", out, sizeof(out)), -1);
  EXPECT_EQ(LIBC_NAMESPACE::b64_pton("Zg==x", out, sizeof(out)), -1);
  // Bits that padding says are absent have to be zero.
  EXPECT_EQ(LIBC_NAMESPACE::b64_pton("Zh==", out, sizeof(out)), -1);
}

TEST(LlvmLibcB64Test, PtonStopsAtTheSizeOfTheTarget) {
  unsigned char out[3];
  EXPECT_EQ(LIBC_NAMESPACE::b64_pton("Zm9v", out, sizeof(out)), 3);
  EXPECT_EQ(LIBC_NAMESPACE::b64_pton("Zm9vYg==", out, sizeof(out)), -1);
  // With no target it only counts.
  EXPECT_EQ(LIBC_NAMESPACE::b64_pton("Zm9vYg==", nullptr, 0), 4);
}

TEST(LlvmLibcB64Test, RoundTrip) {
  unsigned char data[37];
  for (size_t i = 0; i < sizeof(data); ++i)
    data[i] = static_cast<unsigned char>(i * 7 + 3);
  for (size_t length = 0; length <= sizeof(data); ++length) {
    char text[64];
    unsigned char back[64];
    const int n = LIBC_NAMESPACE::b64_ntop(data, length, text, sizeof(text));
    ASSERT_GE(n, 0);
    ASSERT_EQ(LIBC_NAMESPACE::b64_pton(text, back, sizeof(back)), int(length));
    EXPECT_EQ(LIBC_NAMESPACE::inline_memcmp(data, back, length), 0);
  }
}

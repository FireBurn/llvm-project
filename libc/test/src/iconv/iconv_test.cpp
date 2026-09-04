//===-- Unittests for the iconv family ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/types/iconv_t.h"
#include "hdr/types/ssize_t.h"
#include "src/__support/libc_errno.h"
#include "src/iconv/iconv.h"
#include "src/iconv/iconv_close.h"
#include "src/iconv/iconv_open.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcIconvTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {

const iconv_t FAILED = reinterpret_cast<iconv_t>(-1);

// Converts |in| and reports how many bytes came out, or -1.
ssize_t convert(const char *to, const char *from, const char *in, size_t inlen,
                char *out, size_t outlen) {
  iconv_t cd = LIBC_NAMESPACE::iconv_open(to, from);
  if (cd == FAILED)
    return -2;
  char input[64];
  for (size_t i = 0; i < inlen; ++i)
    input[i] = in[i];
  char *ip = input;
  char *op = out;
  size_t il = inlen;
  size_t ol = outlen;
  size_t result = LIBC_NAMESPACE::iconv(cd, &ip, &il, &op, &ol);
  LIBC_NAMESPACE::iconv_close(cd);
  if (result == static_cast<size_t>(-1))
    return -1;
  return op - out;
}

} // anonymous namespace

TEST_F(LlvmLibcIconvTest, OpenAndClose) {
  iconv_t cd = LIBC_NAMESPACE::iconv_open("UTF-8", "ISO-8859-1");
  ASSERT_TRUE(cd != FAILED);
  ASSERT_ERRNO_SUCCESS();
  EXPECT_EQ(LIBC_NAMESPACE::iconv_close(cd), 0);
}

TEST_F(LlvmLibcIconvTest, NamesAreMatchedLoosely) {
  // Case and the punctuation between the parts make no difference, and a
  // trailing behaviour is not part of the name.
  const char *spellings[] = {"ISO-8859-1",     "iso-8859-1", "iso88591",
                             "ISO_8859-1",     "LATIN1",     "latin1",
                             "UTF-8//TRANSLIT"};
  for (const char *name : spellings) {
    iconv_t cd = LIBC_NAMESPACE::iconv_open("UTF-8", name);
    EXPECT_TRUE(cd != FAILED);
    if (cd != FAILED)
      LIBC_NAMESPACE::iconv_close(cd);
  }
}

TEST_F(LlvmLibcIconvTest, AnUnknownSetIsRejected) {
  EXPECT_TRUE(LIBC_NAMESPACE::iconv_open("UTF-8", "NO-SUCH-SET") == FAILED);
  ASSERT_ERRNO_EQ(EINVAL);
  EXPECT_TRUE(LIBC_NAMESPACE::iconv_open("NO-SUCH-SET", "UTF-8") == FAILED);
  ASSERT_ERRNO_EQ(EINVAL);
}

TEST_F(LlvmLibcIconvTest, Latin1ToUtf8) {
  char out[16] = {};
  // 0xE9 is e with an acute in Latin-1, two bytes in UTF-8.
  ASSERT_EQ(convert("UTF-8", "ISO-8859-1", "\xe9", 1, out, sizeof(out)),
            ssize_t(2));
  EXPECT_EQ(static_cast<unsigned>(static_cast<unsigned char>(out[0])), 0xC3u);
  EXPECT_EQ(static_cast<unsigned>(static_cast<unsigned char>(out[1])), 0xA9u);
}

TEST_F(LlvmLibcIconvTest, Utf8ToLatin1) {
  char out[16] = {};
  ASSERT_EQ(convert("ISO-8859-1", "UTF-8", "\xc3\xa9", 2, out, sizeof(out)),
            ssize_t(1));
  EXPECT_EQ(static_cast<unsigned>(static_cast<unsigned char>(out[0])), 0xE9u);
}

TEST_F(LlvmLibcIconvTest, Utf16AndUtf32) {
  char out[16] = {};
  // "A" in UTF-16LE is 41 00.
  ASSERT_EQ(convert("UTF-16LE", "UTF-8", "A", 1, out, sizeof(out)), ssize_t(2));
  EXPECT_EQ(out[0], 'A');
  EXPECT_EQ(out[1], '\0');

  // The same in big endian order.
  ASSERT_EQ(convert("UTF-16BE", "UTF-8", "A", 1, out, sizeof(out)), ssize_t(2));
  EXPECT_EQ(out[0], '\0');
  EXPECT_EQ(out[1], 'A');

  ASSERT_EQ(convert("UTF-32LE", "UTF-8", "A", 1, out, sizeof(out)), ssize_t(4));
  EXPECT_EQ(out[0], 'A');
}

TEST_F(LlvmLibcIconvTest, SurrogatePairs) {
  char out[16] = {};
  // U+1F600 needs a surrogate pair in UTF-16 and four bytes in UTF-8.
  ASSERT_EQ(
      convert("UTF-16LE", "UTF-8", "\xf0\x9f\x98\x80", 4, out, sizeof(out)),
      ssize_t(4));
  EXPECT_EQ(static_cast<unsigned>(static_cast<unsigned char>(out[0])), 0x3Du);
  EXPECT_EQ(static_cast<unsigned>(static_cast<unsigned char>(out[1])), 0xD8u);

  // And back again.
  char back[16] = {};
  ASSERT_EQ(convert("UTF-8", "UTF-16LE", out, 4, back, sizeof(back)),
            ssize_t(4));
  EXPECT_EQ(static_cast<unsigned>(static_cast<unsigned char>(back[0])), 0xF0u);
}

TEST_F(LlvmLibcIconvTest, RoundTripsThroughEverySet) {
  const char *sets[] = {"UTF-8",    "UTF-16LE",   "UTF-16BE", "UTF-32LE",
                        "UTF-32BE", "ISO-8859-1", "CP1252"};
  for (const char *set : sets) {
    char middle[32] = {};
    char back[32] = {};
    ssize_t n = convert(set, "UTF-8", "Hello", 5, middle, sizeof(middle));
    ASSERT_GT(n, ssize_t(0));
    ASSERT_EQ(convert("UTF-8", set, middle, static_cast<size_t>(n), back,
                      sizeof(back)),
              ssize_t(5));
    EXPECT_EQ(back[0], 'H');
    EXPECT_EQ(back[4], 'o');
  }
}

TEST_F(LlvmLibcIconvTest, InvalidInput) {
  char out[16] = {};
  // A continuation byte with no lead byte before it.
  EXPECT_EQ(convert("UTF-8", "UTF-8", "\x80", 1, out, sizeof(out)),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(EILSEQ);

  // A character written in more bytes than it needs.
  EXPECT_EQ(convert("UTF-8", "UTF-8", "\xc0\x80", 2, out, sizeof(out)),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(EILSEQ);
}

TEST_F(LlvmLibcIconvTest, IncompleteInput) {
  char out[16] = {};
  // The first byte of a two byte character, with nothing after it.
  EXPECT_EQ(convert("ISO-8859-1", "UTF-8", "\xc3", 1, out, sizeof(out)),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(EINVAL);
}

TEST_F(LlvmLibcIconvTest, OutputTooSmall) {
  char out[1] = {};
  // One byte of room, but the character needs two.
  EXPECT_EQ(convert("UTF-8", "ISO-8859-1", "\xe9", 1, out, sizeof(out)),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(E2BIG);
}

TEST_F(LlvmLibcIconvTest, ACharacterWithNoPlaceInTheTargetSet) {
  char out[16] = {};
  // A CJK ideograph cannot be written in Latin-1.
  EXPECT_EQ(convert("ISO-8859-1", "UTF-8", "\xe4\xb8\x80", 3, out, sizeof(out)),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(EILSEQ);

  // Nor can a byte which the source set does not assign be read.
  EXPECT_EQ(convert("UTF-8", "ISO-8859-3", "\xa5", 1, out, sizeof(out)),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(EILSEQ);
}

TEST_F(LlvmLibcIconvTest, PartialProgressIsKept) {
  iconv_t cd = LIBC_NAMESPACE::iconv_open("UTF-8", "ISO-8859-1");
  ASSERT_TRUE(cd != FAILED);

  char input[] = "\xe9\xe8";
  char out[3] = {};
  char *ip = input;
  char *op = out;
  size_t il = 2;
  size_t ol = 3;
  // Room for the first character and one byte of the second.
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, &ip, &il, &op, &ol),
            static_cast<size_t>(-1));
  ASSERT_ERRNO_EQ(E2BIG);
  // The first character was converted and only it was consumed.
  EXPECT_EQ(il, size_t(1));
  EXPECT_EQ(static_cast<long>(op - out), 2L);

  ASSERT_EQ(LIBC_NAMESPACE::iconv_close(cd), 0);
}

TEST_F(LlvmLibcIconvTest, BadDescriptor) {
  char *ip = nullptr;
  size_t il = 0;
  char *op = nullptr;
  size_t ol = 0;
  EXPECT_EQ(LIBC_NAMESPACE::iconv(FAILED, &ip, &il, &op, &ol),
            static_cast<size_t>(-1));
  ASSERT_ERRNO_EQ(EBADF);
  EXPECT_EQ(LIBC_NAMESPACE::iconv_close(FAILED), -1);
  ASSERT_ERRNO_EQ(EBADF);
}

TEST_F(LlvmLibcIconvTest, ResettingTheStateDoesNothing) {
  iconv_t cd = LIBC_NAMESPACE::iconv_open("UTF-8", "ISO-8859-1");
  ASSERT_TRUE(cd != FAILED);
  // None of these conversions carry state, so there is nothing to reset.
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, nullptr, nullptr, nullptr, nullptr),
            size_t(0));
  ASSERT_EQ(LIBC_NAMESPACE::iconv_close(cd), 0);
}

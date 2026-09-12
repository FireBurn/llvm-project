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
#include "src/__support/endian_internal.h"
#include "src/__support/libc_errno.h"
#include "src/iconv/iconv.h"
#include "src/iconv/iconv_close.h"
#include "src/iconv/iconv_open.h"
#include "src/string/string_utils.h"
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

TEST_F(LlvmLibcIconvTest, Ucs4IsBigEndian) {
  char out[16] = {};
  ASSERT_EQ(convert("UCS-4", "UTF-8", "A", 1, out, sizeof(out)), ssize_t(4));
  EXPECT_EQ(out[0], '\0');
  EXPECT_EQ(out[3], 'A');
}

TEST_F(LlvmLibcIconvTest, Ucs2) {
  char out[16] = {};
  // "A" in each byte order, and plain UCS-2 in the host's.
  ASSERT_EQ(convert("UCS-2LE", "UTF-8", "A", 1, out, sizeof(out)), ssize_t(2));
  EXPECT_EQ(out[0], 'A');
  ASSERT_EQ(convert("UCS-2BE", "UTF-8", "A", 1, out, sizeof(out)), ssize_t(2));
  EXPECT_EQ(out[1], 'A');
  ASSERT_EQ(convert("UCS-2", "UTF-8", "A", 1, out, sizeof(out)), ssize_t(2));
  EXPECT_EQ(out[LIBC_NAMESPACE::Endian::IS_LITTLE ? 0 : 1], 'A');

  // UCS-2 has no surrogates, so a character past U+FFFF has no place in it,
  // and a surrogate in its input is not a character.
  EXPECT_EQ(
      convert("UCS-2LE", "UTF-8", "\xf0\x9f\x98\x80", 4, out, sizeof(out)),
      ssize_t(-1));
  ASSERT_ERRNO_EQ(EILSEQ);
  EXPECT_EQ(
      convert("UTF-8", "UCS-2LE", "\x3d\xd8\x00\xde", 4, out, sizeof(out)),
      ssize_t(-1));
  ASSERT_ERRNO_EQ(EILSEQ);
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

TEST_F(LlvmLibcIconvTest, ABrokenCharacterIsInvalidBeforeItsEnd) {
  char out[16] = {};
  // 0x41 cannot continue the character 0xE2 starts, so the input is wrong even
  // though it stops short of where the character would end.
  EXPECT_EQ(convert("UTF-8", "UTF-8", "\xe2\x41", 2, out, sizeof(out)),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(EILSEQ);

  // 0xC0 never starts a character.
  EXPECT_EQ(convert("UTF-8", "UTF-8", "\xc0", 1, out, sizeof(out)),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(EILSEQ);

  // A beginning which more input could still complete is only incomplete.
  EXPECT_EQ(convert("UTF-8", "UTF-8", "\xf0\x9f", 2, out, sizeof(out)),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(EINVAL);
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

TEST_F(LlvmLibcIconvTest, TheOrderErrorsAreReportedIn) {
  char out[1] = {};
  // Input which is not a character is reported even with no room left.
  EXPECT_EQ(convert("UTF-8", "UTF-8", "a\xff", 2, out, sizeof(out)),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(EILSEQ);

  // A full output is reported before a character with no place in the target.
  EXPECT_EQ(convert("ASCII", "UTF-8", "a\xc3\xa9", 3, out, sizeof(out)),
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

  // Nor the replacement character, which the table marks those bytes with.
  EXPECT_EQ(convert("ISO-8859-3", "UTF-8", "\xef\xbf\xbd", 3, out, sizeof(out)),
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

// Converts |in| from UTF-8 with the whole input offered at once, and reports
// what came out and how much input was used.
struct Result {
  size_t ret;
  int error;
  size_t used;
  size_t made;
};

Result convert_all(const char *to, const char *in, size_t inlen, char *out,
                   size_t outlen) {
  iconv_t cd = LIBC_NAMESPACE::iconv_open(to, "UTF-8");
  if (cd == FAILED)
    return {0, -1, 0, 0};
  char input[64];
  for (size_t i = 0; i < inlen; ++i)
    input[i] = in[i];
  char *ip = input;
  char *op = out;
  size_t il = inlen;
  size_t ol = outlen;
  size_t ret = LIBC_NAMESPACE::iconv(cd, &ip, &il, &op, &ol);
  int error = ret == static_cast<size_t>(-1) ? libc_errno : 0;
  libc_errno = 0;
  LIBC_NAMESPACE::iconv_close(cd);
  return {ret, error, inlen - il, static_cast<size_t>(op - out)};
}

TEST_F(LlvmLibcIconvTest, IgnoreLeavesOutWhatHasNoPlace) {
  char out[16] = {};
  // e with an acute has no place in ASCII. The rest is converted, all of the
  // input is used, and the call still reports that something was left out.
  Result r = convert_all("ASCII//IGNORE",
                         "a\xc3\xa9"
                         "b",
                         4, out, sizeof(out));
  EXPECT_EQ(r.ret, static_cast<size_t>(-1));
  EXPECT_EQ(r.error, EILSEQ);
  EXPECT_EQ(r.used, size_t(4));
  ASSERT_EQ(r.made, size_t(2));
  EXPECT_EQ(out[0], 'a');
  EXPECT_EQ(out[1], 'b');
}

TEST_F(LlvmLibcIconvTest, IgnoreSkipsInputWhichIsNotACharacter) {
  struct Case {
    const char *in;
    size_t len;
    const char *expected;
  };
  // A broken character is passed over as far as it got, and one which is
  // complete but not allowed is passed over whole.
  const Case cases[] = {
      {"a\x80"
       "b",
       3, "ab"}, // A continuation byte on its own.
      {"a\xe2\x82"
       "Ab",
       5, "aAb"}, // Cut short by an ASCII byte.
      {"a\xc0\x80"
       "b",
       4, "ab"}, // 0xC0 never starts a character.
      {"a\xed\xa0\x80"
       "b",
       5, "ab"}, // A surrogate.
      {"a\xf4\x90\x80\x80"
       "b",
       6, "ab"}, // Past the end of Unicode.
      {"a\xff"
       "b",
       3, "ab"},
  };
  for (const Case &c : cases) {
    char out[16] = {};
    Result r = convert_all("UTF-8//IGNORE", c.in, c.len, out, sizeof(out));
    EXPECT_EQ(r.error, EILSEQ);
    EXPECT_EQ(r.used, c.len);
    ASSERT_EQ(r.made, LIBC_NAMESPACE::internal::string_length(c.expected));
    for (size_t i = 0; i < r.made; ++i)
      EXPECT_EQ(out[i], c.expected[i]);
  }
}

TEST_F(LlvmLibcIconvTest, IgnoreStillReportsAnIncompleteEnd) {
  char out[16] = {};
  Result r = convert_all("ASCII//IGNORE", "a\xc3", 2, out, sizeof(out));
  EXPECT_EQ(r.error, EINVAL);
  EXPECT_EQ(r.used, size_t(1));
}

TEST_F(LlvmLibcIconvTest, IgnoreIsOnlyReadFromTheTargetName) {
  char out[16] = {};
  // Spelled in any case, and among other flags.
  const char *spellings[] = {"ASCII//IGNORE", "ASCII//ignore",
                             "ASCII//TRANSLIT,IGNORE", "ASCII//IGNORE//"};
  for (const char *to : spellings)
    EXPECT_EQ(convert_all(to, "\xc3\xa9", 2, out, sizeof(out)).used, size_t(2));

  // On the source set's name it does nothing.
  iconv_t cd = LIBC_NAMESPACE::iconv_open("ASCII", "UTF-8//IGNORE");
  ASSERT_TRUE(cd != FAILED);
  char input[] = "\xc3\xa9";
  char *ip = input;
  char *op = out;
  size_t il = 2;
  size_t ol = sizeof(out);
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, &ip, &il, &op, &ol),
            static_cast<size_t>(-1));
  ASSERT_ERRNO_EQ(EILSEQ);
  EXPECT_EQ(il, size_t(2));
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

Result step(iconv_t cd, const char *in, size_t inlen, char *out,
            size_t outlen) {
  char input[16];
  for (size_t i = 0; i < inlen; ++i)
    input[i] = in[i];
  char *ip = input;
  char *op = out;
  size_t il = inlen;
  size_t ol = outlen;
  size_t ret = LIBC_NAMESPACE::iconv(cd, &ip, &il, &op, &ol);
  int error = ret == static_cast<size_t>(-1) ? libc_errno : 0;
  libc_errno = 0;
  return {ret, error, inlen - il, outlen - ol};
}

// Where the high and low bytes of a two byte unit go in the host's order.
const size_t HIGH = LIBC_NAMESPACE::Endian::IS_LITTLE ? 1 : 0;
const size_t LOW = 1 - HIGH;

unsigned at(const char *p, size_t i) {
  return static_cast<unsigned char>(p[i]);
}

TEST_F(LlvmLibcIconvTest, Utf16WritesAByteOrderMarkFirst) {
  iconv_t cd = LIBC_NAMESPACE::iconv_open("UTF-16", "UTF-8");
  ASSERT_TRUE(cd != FAILED);
  char out[16] = {};
  // The mark, then "A", both in the host's byte order.
  Result r = step(cd, "A", 1, out, sizeof(out));
  ASSERT_EQ(r.made, size_t(4));
  EXPECT_EQ(at(out, HIGH), 0xFEu);
  EXPECT_EQ(at(out, LOW), 0xFFu);
  EXPECT_EQ(at(out, 2 + LOW), unsigned('A'));

  // Only once.
  r = step(cd, "B", 1, out, sizeof(out));
  ASSERT_EQ(r.made, size_t(2));
  EXPECT_EQ(at(out, LOW), unsigned('B'));

  // Going back to the initial state means the next output has one again.
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, nullptr, nullptr, nullptr, nullptr),
            size_t(0));
  EXPECT_EQ(step(cd, "C", 1, out, sizeof(out)).made, size_t(4));
  ASSERT_EQ(LIBC_NAMESPACE::iconv_close(cd), 0);

  // UTF-32 writes one as well.
  cd = LIBC_NAMESPACE::iconv_open("UTF-32", "UTF-8");
  ASSERT_TRUE(cd != FAILED);
  EXPECT_EQ(step(cd, "A", 1, out, sizeof(out)).made, size_t(8));
  ASSERT_EQ(LIBC_NAMESPACE::iconv_close(cd), 0);
}

TEST_F(LlvmLibcIconvTest, AByteOrderMarkStaysWhenTheCharacterDoesNotFit) {
  iconv_t cd = LIBC_NAMESPACE::iconv_open("UTF-16", "UTF-8");
  ASSERT_TRUE(cd != FAILED);
  char out[2] = {};
  // Room for the mark but not for the character after it.
  Result r = step(cd, "A", 1, out, sizeof(out));
  EXPECT_EQ(r.error, E2BIG);
  EXPECT_EQ(r.used, size_t(0));
  EXPECT_EQ(r.made, size_t(2));

  // The retry writes the character without a second mark.
  r = step(cd, "A", 1, out, sizeof(out));
  EXPECT_EQ(r.error, 0);
  ASSERT_EQ(r.made, size_t(2));
  EXPECT_EQ(at(out, LOW), unsigned('A'));
  ASSERT_EQ(LIBC_NAMESPACE::iconv_close(cd), 0);
}

TEST_F(LlvmLibcIconvTest, ReadingAByteOrderMark) {
  char out[16] = {};
  // The mark gives the order and is not itself converted.
  EXPECT_EQ(convert("UTF-8", "UTF-16", "\xfe\xff\x00\x41", 4, out, sizeof(out)),
            ssize_t(1));
  EXPECT_EQ(out[0], 'A');
  EXPECT_EQ(convert("UTF-8", "UTF-16", "\xff\xfe\x41\x00", 4, out, sizeof(out)),
            ssize_t(1));
  EXPECT_EQ(out[0], 'A');
  EXPECT_EQ(convert("UTF-8", "UTF-32", "\x00\x00\xfe\xff\x00\x00\x00\x41", 8,
                    out, sizeof(out)),
            ssize_t(1));
  EXPECT_EQ(out[0], 'A');

  // Without one, the input is in the host's order.
  char host[2] = {};
  host[LOW] = 'A';
  EXPECT_EQ(convert("UTF-8", "UTF-16", host, 2, out, sizeof(out)), ssize_t(1));
  EXPECT_EQ(out[0], 'A');

  // Past the first character, FEFF is the character U+FEFF.
  EXPECT_EQ(convert("UTF-8", "UTF-16", "\xfe\xff\x00\x41\xfe\xff", 6, out,
                    sizeof(out)),
            ssize_t(4));
  EXPECT_EQ(at(out, 1), 0xEFu);
}

TEST_F(LlvmLibcIconvTest, ResettingLetsAByteOrderMarkBeReadAgain) {
  iconv_t cd = LIBC_NAMESPACE::iconv_open("UTF-8", "UTF-16");
  ASSERT_TRUE(cd != FAILED);
  char out[16] = {};
  // A mark cut short is incomplete, and a whole one on its own is read.
  Result r = step(cd, "\xfe", 1, out, sizeof(out));
  EXPECT_EQ(r.error, EINVAL);
  EXPECT_EQ(r.used, size_t(0));
  r = step(cd, "\xfe\xff", 2, out, sizeof(out));
  EXPECT_EQ(r.used, size_t(2));
  EXPECT_EQ(r.made, size_t(0));

  // After a reset another mark may come, and until one does the order the
  // last one gave still holds.
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, nullptr, nullptr, nullptr, nullptr),
            size_t(0));
  r = step(cd, "\x00\x41", 2, out, sizeof(out));
  ASSERT_EQ(r.made, size_t(1));
  EXPECT_EQ(out[0], 'A');
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, nullptr, nullptr, nullptr, nullptr),
            size_t(0));
  r = step(cd, "\xff\xfe\x42\x00", 4, out, sizeof(out));
  ASSERT_EQ(r.made, size_t(1));
  EXPECT_EQ(out[0], 'B');
  ASSERT_EQ(LIBC_NAMESPACE::iconv_close(cd), 0);
}

TEST_F(LlvmLibcIconvTest, UnicodeIsUcs2WithAByteOrderMark) {
  char out[16] = {};
  EXPECT_EQ(convert("UNICODE", "UTF-8", "A", 1, out, sizeof(out)), ssize_t(4));

  // The mark is written before a character the set cannot hold is found.
  iconv_t cd = LIBC_NAMESPACE::iconv_open("UNICODE", "UTF-8");
  ASSERT_TRUE(cd != FAILED);
  Result r = step(cd, "\xf0\x9f\x98\x80", 4, out, sizeof(out));
  EXPECT_EQ(r.error, EILSEQ);
  EXPECT_EQ(r.made, size_t(2));
  ASSERT_EQ(LIBC_NAMESPACE::iconv_close(cd), 0);
}

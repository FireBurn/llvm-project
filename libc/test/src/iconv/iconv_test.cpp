//===-- Unittests for the iconv family ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/locale_macros.h"
#include "hdr/types/iconv_t.h"
#include "hdr/types/ssize_t.h"
#include "src/__support/endian_internal.h"
#include "src/__support/libc_errno.h"
#include "src/iconv/iconv.h"
#include "src/iconv/iconv_close.h"
#include "src/iconv/iconv_open.h"
#include "src/locale/setlocale.h"
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

TEST_F(LlvmLibcIconvTest, OtherNamesForTheSameSets) {
  struct Case {
    const char *name;
    const char *bytes; // "A" in the set.
    size_t length;
  };
  const bool little = LIBC_NAMESPACE::Endian::IS_LITTLE;
  const Case cases[] = {
      {"csASCII", "A", 1},
      {"L1", "A", 1},
      {"ISO-IR-100", "A", 1},
      {"UNICODELITTLE", "A\0", 2},
      {"UNICODEBIG", "\0A", 2},
      {"ISO-10646-UCS-4", "\0\0\0A", 4},
      {"UCS-2-INTERNAL", little ? "A\0" : "\0A", 2},
      {"UCS-2-SWAPPED", little ? "\0A" : "A\0", 2},
      {"UCS-4-SWAPPED", little ? "\0\0\0A" : "A\0\0\0", 4},
      {"WCHAR_T", little ? "A\0\0\0" : "\0\0\0A", 4},
      {"WINDOWS-1258", "A", 1},
      {"MS-HEBR", "A", 1},
      {"TCVN5712-1:1993", "A", 1},
  };
  for (const Case &c : cases) {
    char out[8] = {};
    ASSERT_EQ(convert(c.name, "UTF-8", "A", 1, out, sizeof(out)),
              static_cast<ssize_t>(c.length));
    for (size_t i = 0; i < c.length; ++i)
      EXPECT_EQ(out[i], c.bytes[i]);
  }
}

TEST_F(LlvmLibcIconvTest, TheLocalesOwnSet) {
  // An empty name, and libiconv's "CHAR", are the set of the locale in force
  // when the conversion is opened. In the C locale that is ASCII.
  const char *names[] = {"", "CHAR"};
  char out[16] = {};
  for (const char *name : names) {
    EXPECT_EQ(convert("UTF-16LE", name, "A\xc3\xa9", 3, out, sizeof(out)),
              ssize_t(-1));
    ASSERT_ERRNO_EQ(EILSEQ);
  }

  ASSERT_TRUE(LIBC_NAMESPACE::setlocale(LC_ALL, "C.UTF-8") != nullptr);
  for (const char *name : names)
    EXPECT_EQ(convert("UTF-16LE", name, "A\xc3\xa9", 3, out, sizeof(out)),
              ssize_t(4));
  LIBC_NAMESPACE::setlocale(LC_ALL, "C");
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

TEST_F(LlvmLibcIconvTest, MoreSingleByteSets) {
  struct Case {
    const char *name;
    char byte;
    const char *utf8;
    size_t length;
  };
  const Case cases[] = {
      // Latin capital letter r with acute.
      {"CP1250", '\xc0', "\xc5\x94", 2},
      // Greek small letter iota with dialytika and tonos.
      {"WINDOWS-1253", '\xc0', "\xce\x90", 2},
      // Latin capital letter a with grave.
      {"CP1254", '\xc0', "\xc3\x80", 2},
      // Arabic letter heh goal.
      {"MS-ARAB", '\xc0', "\xdb\x81", 2},
      // Latin capital letter a with ogonek.
      {"CP1257", '\xc0', "\xc4\x84", 2},
      // Thai character pho samphao.
      {"WINDOWS-874", '\xc0', "\xe0\xb8\xa0", 3},
      // Box drawings light up and right.
      {"IBM862", '\xc0', "\xe2\x94\x94", 3},
      // Box drawings light up and right.
      {"CP866", '\xc0', "\xe2\x94\x94", 3},
      // Cyrillic small letter yu.
      {"KOI8-U", '\xc0', "\xd1\x8e", 2},
      // Cyrillic capital letter a.
      {"KZ-1048", '\xc0', "\xd0\x90", 2},
      // Cyrillic small letter yu.
      {"KOI8-T", '\xc0', "\xd1\x8e", 2},
      // Cyrillic small letter yu.
      {"KOI8-RU", '\xc0', "\xd1\x8e", 2},
      // Cyrillic capital letter a.
      {"PTCP154", '\xc0', "\xd0\x90", 2},
      // Georgian letter an.
      {"GEORGIAN-ACADEMY", '\xc0', "\xe1\x83\x90", 3},
      // Georgian letter an.
      {"GEORGIAN-PS", '\xc0', "\xe1\x83\x90", 3},
      // Latin small letter a with circumflex.
      {"ROMAN8", '\xc0', "\xc3\xa2", 2},
      // Box drawings light up and right.
      {"CP1131", '\xc0', "\xe2\x94\x94", 3},
      // No-break space.
      {"MULELAO-1", '\xa0', "\xc2\xa0", 2},
      // Lao vowel sign a.
      {"IBM-CP1133", '\xc0', "\xe0\xba\xb0", 3},
      // Thai character pho samphao.
      {"TIS620", '\xc0', "\xe0\xb8\xa0", 3},
  };
  for (const Case &c : cases) {
    char out[8] = {};
    ASSERT_EQ(convert("UTF-8", c.name, &c.byte, 1, out, sizeof(out)),
              static_cast<ssize_t>(c.length));
    for (size_t i = 0; i < c.length; ++i)
      EXPECT_EQ(out[i], c.utf8[i]);
    char back[8] = {};
    ASSERT_EQ(convert(c.name, "UTF-8", c.utf8, c.length, back, sizeof(back)),
              ssize_t(1));
    EXPECT_EQ(back[0], c.byte);
  }
}

TEST_F(LlvmLibcIconvTest, SetsWhichAreNotAsciiBelow0x80) {
  struct Case {
    const char *name;
    char byte;
    const char *utf8;
    size_t length;
  };
  const Case cases[] = {
      // Superscript one.
      {"NEXTSTEP", '\xc0', "\xc2\xb9", 2},
      // Armenian capital letter et.
      {"ARMSCII-8", '\xc0', "\xd4\xb8", 2},
      // Yen sign.
      {"ISO646-JP", '\x5c', "\xc2\xa5", 2},
      // Yen sign.
      {"ISO646-CN", '\x24', "\xc2\xa5", 2},
      // Latin capital letter a with breve and hook above.
      {"VISCII", '\x02', "\xe1\xba\xb2", 3},
      // Latin capital letter a with grave.
      {"VISCII", '\xc0', "\xc3\x80", 2},
      // Yen sign.
      {"JISX0201-1976", '\x5c', "\xc2\xa5", 2},
      // Halfwidth katakana letter ta.
      {"JISX0201-1976", '\xc0', "\xef\xbe\x80", 3},
  };
  for (const Case &c : cases) {
    char out[8] = {};
    ASSERT_EQ(convert("UTF-8", c.name, &c.byte, 1, out, sizeof(out)),
              static_cast<ssize_t>(c.length));
    for (size_t i = 0; i < c.length; ++i)
      EXPECT_EQ(out[i], c.utf8[i]);
    char back[8] = {};
    ASSERT_EQ(convert(c.name, "UTF-8", c.utf8, c.length, back, sizeof(back)),
              ssize_t(1));
    EXPECT_EQ(back[0], c.byte);
  }

  // A seven bit set has nothing from 0x80 up, and no place for the ASCII
  // character it replaces.
  char out[8] = {};
  EXPECT_EQ(convert("UTF-8", "ISO646-JP", "\x80", 1, out, sizeof(out)),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(EILSEQ);
  EXPECT_EQ(convert("ISO646-JP", "UTF-8", "\\", 1, out, sizeof(out)),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(EILSEQ);
}

TEST_F(LlvmLibcIconvTest, MacOsSets) {
  struct Case {
    const char *name;
    char byte;
    const char *utf8;
    size_t length;
  };
  // The Mac OS sets follow Apple's current tables, so MacRoman has the euro
  // sign at 0xDB, and MacUkraine keeps the currency sign MacCyrillic replaced.
  const Case cases[] = {
      // Euro sign.
      {"MACINTOSH", '\xdb', "\xe2\x82\xac", 3},
      // Latin small letter n with cedilla.
      {"MacCentralEurope", '\xc0', "\xc5\x86", 2},
      // Inverted question mark.
      {"MacIceland", '\xc0', "\xc2\xbf", 2},
      // Inverted question mark.
      {"MacCroatian", '\xc0', "\xc2\xbf", 2},
      // Latin capital letter s with comma below.
      {"MacRomania", '\xaf', "\xc8\x98", 2},
      // Euro sign.
      {"MacCyrillic", '\xff', "\xe2\x82\xac", 3},
      // Currency sign.
      {"MAC-UK", '\xff', "\xc2\xa4", 2},
      // Greek small letter alpha with tonos.
      {"MacGreek", '\xc0', "\xce\xac", 2},
      // Inverted question mark.
      {"MacTurkish", '\xc0', "\xc2\xbf", 2},
      // Arabic letter beh.
      {"MacArabic", '\xc8', "\xd8\xa8", 2},
  };
  for (const Case &c : cases) {
    char out[8] = {};
    ASSERT_EQ(convert("UTF-8", c.name, &c.byte, 1, out, sizeof(out)),
              static_cast<ssize_t>(c.length));
    for (size_t i = 0; i < c.length; ++i)
      EXPECT_EQ(out[i], c.utf8[i]);
    char back[8] = {};
    ASSERT_EQ(convert(c.name, "UTF-8", c.utf8, c.length, back, sizeof(back)),
              ssize_t(1));
    EXPECT_EQ(back[0], c.byte);
  }
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

// Converts |in| and checks the bytes that come out against |expected|.
void expect_bytes(const char *to, const char *from, const char *in,
                  size_t inlen, const char *expected, size_t expected_len) {
  char out[64] = {};
  ASSERT_EQ(convert(to, from, in, inlen, out, sizeof(out)),
            static_cast<ssize_t>(expected_len));
  for (size_t i = 0; i < expected_len; ++i)
    EXPECT_EQ(out[i], expected[i]);
}

TEST_F(LlvmLibcIconvTest, WritingC99AndJavaEscapes) {
  // "A", e with an acute, the euro sign, U+1F600 and a backslash.
  const char in[] = "A\xc3\xa9\xe2\x82\xac\xf0\x9f\x98\x80\\";
  const char c99[] = "A\\u00e9\\u20ac\\U0001f600\\";
  const char java[] = "A\\u00e9\\u20ac\\ud83d\\ude00\\";
  expect_bytes("C99", "UTF-8", in, sizeof(in) - 1, c99, sizeof(c99) - 1);
  expect_bytes("JAVA", "UTF-8", in, sizeof(in) - 1, java, sizeof(java) - 1);

  // C99 cannot name a character below U+00A0, so it is written as it is.
  expect_bytes("C99", "UTF-8", "\xc2\x85", 2, "\x85", 1);
  expect_bytes("JAVA", "UTF-8", "\xc2\x85", 2, "\\u0085", 6);
}

TEST_F(LlvmLibcIconvTest, ReadingC99AndJavaEscapes) {
  expect_bytes("UTF-8", "C99", "\\u00E9", 6, "\xc3\xa9", 2);
  expect_bytes("UTF-8", "C99", "\\U0001F600", 10, "\xf0\x9f\x98\x80", 4);
  expect_bytes("UTF-8", "JAVA", "\\ud83d\\ude00", 12, "\xf0\x9f\x98\x80", 4);
  expect_bytes("UTF-8", "JAVA", "\\u0041", 6, "A", 1);

  // What is not an escape is text: a backslash before anything else, a
  // malformed escape, and in Java a surrogate on its own.
  expect_bytes("UTF-8", "C99", "\\xg", 3, "\\xg", 3);
  expect_bytes("UTF-8", "C99", "\\u00eg", 6, "\\u00eg", 6);
  expect_bytes("UTF-8", "JAVA", "\\ude00", 6, "\\ude00", 6);

  // An escape cut short may still be completed.
  char out[16] = {};
  EXPECT_EQ(convert("UTF-8", "C99", "\\u00e", 5, out, sizeof(out)),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(EINVAL);
  EXPECT_EQ(convert("UTF-8", "JAVA", "\\ud83d\\u", 8, out, sizeof(out)),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(EINVAL);

  // C99 names no surrogate and nothing below U+00A0 but $, @ and `.
  EXPECT_EQ(convert("UTF-8", "C99", "\\ud83d", 6, out, sizeof(out)),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(EILSEQ);
  EXPECT_EQ(convert("UTF-8", "C99", "\\u0041", 6, out, sizeof(out)),
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

Result convert_all_from(const char *to, const char *from, const char *in,
                        size_t inlen, char *out, size_t outlen) {
  iconv_t cd = LIBC_NAMESPACE::iconv_open(to, from);
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

TEST_F(LlvmLibcIconvTest, WritingUtf7) {
  iconv_t cd = LIBC_NAMESPACE::iconv_open("UTF-7", "UTF-8");
  ASSERT_TRUE(cd != FAILED);
  char out[32] = {};
  // "Hi+€A€." A plus sign is "+-", the euro sign is a base64 run, which a '-'
  // ends before "A" but not before ".".
  const char in[] = "Hi+\xe2\x82\xac"
                    "A\xe2\x82\xac.";
  const char expected[] = "Hi+-+IKw-A+IKw.";
  Result r = step(cd, in, sizeof(in) - 1, out, sizeof(out));
  EXPECT_EQ(r.error, 0);
  ASSERT_EQ(r.made, sizeof(expected) - 1);
  for (size_t i = 0; i < r.made; ++i)
    EXPECT_EQ(out[i], expected[i]);

  // A run still open when the state is reset is ended, if there is room.
  r = step(cd, "\xe2\x82\xac", 3, out, sizeof(out));
  ASSERT_EQ(r.made, size_t(3));
  char *op = out;
  size_t ol = 1;
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, nullptr, nullptr, &op, &ol),
            static_cast<size_t>(-1));
  ASSERT_ERRNO_EQ(E2BIG);
  ol = sizeof(out);
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, nullptr, nullptr, &op, &ol), size_t(0));
  EXPECT_EQ(static_cast<long>(op - out), 2L);
  EXPECT_EQ(out[0], 'w');
  EXPECT_EQ(out[1], '-');
  ASSERT_EQ(LIBC_NAMESPACE::iconv_close(cd), 0);
}

TEST_F(LlvmLibcIconvTest, ReadingUtf7) {
  expect_bytes("UTF-8", "UTF-7", "+IKw-A", 6,
               "\xe2\x82\xac"
               "A",
               4);
  expect_bytes("UTF-8", "UTF-7", "+IKw.A", 6, "\xe2\x82\xac.A", 5);
  expect_bytes("UTF-8", "UTF-7", "+-", 2, "+", 1);
  expect_bytes("UTF-8", "UTF-7", "+2D3eAA-", 8, "\xf0\x9f\x98\x80", 4);

  // A run may go on into the next call, halfway through a surrogate pair.
  iconv_t cd = LIBC_NAMESPACE::iconv_open("UTF-8", "UTF-7");
  ASSERT_TRUE(cd != FAILED);
  char out[16] = {};
  Result r = step(cd, "+2D3", 4, out, sizeof(out));
  EXPECT_EQ(r.used, size_t(4));
  EXPECT_EQ(r.made, size_t(0));
  r = step(cd, "eAA-", 4, out, sizeof(out));
  EXPECT_EQ(r.made, size_t(4));
  ASSERT_EQ(LIBC_NAMESPACE::iconv_close(cd), 0);
}

TEST_F(LlvmLibcIconvTest, Utf7Errors) {
  char out[16] = {};
  // A low surrogate on its own is not a character, and nothing of it is used.
  Result r = convert_all_from("UTF-8", "UTF-7", "+3gB", 4, out, sizeof(out));
  EXPECT_EQ(r.error, EILSEQ);
  EXPECT_EQ(r.used, size_t(0));
  // Leftover bits which are not zero are reported where the run ends.
  r = convert_all_from("UTF-8", "UTF-7", "+II-", 4, out, sizeof(out));
  EXPECT_EQ(r.error, EILSEQ);
  EXPECT_EQ(r.used, size_t(3));
  // A backslash is not a UTF-7 character, and a '+' at the end may yet start
  // a run.
  r = convert_all_from("UTF-8", "UTF-7", "\\", 1, out, sizeof(out));
  EXPECT_EQ(r.error, EILSEQ);
  r = convert_all_from("UTF-8", "UTF-7", "A+", 2, out, sizeof(out));
  EXPECT_EQ(r.error, EINVAL);
  EXPECT_EQ(r.used, size_t(1));
  // Under //IGNORE a bad unit is dropped with the rest of its run.
  r = convert_all_from("UTF-8//IGNORE", "UTF-7", "+3gBAB-C", 8, out,
                       sizeof(out));
  EXPECT_EQ(r.used, size_t(8));
  ASSERT_EQ(r.made, size_t(1));
  EXPECT_EQ(out[0], 'C');
}

// Converts |in|, then goes back to the initial state with room to write what
// that owes, and reports how many bytes came out, or -1.
ssize_t convert_and_finish(const char *to, const char *from, const char *in,
                           size_t inlen, char *out, size_t outlen) {
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
  if (result != static_cast<size_t>(-1))
    result = LIBC_NAMESPACE::iconv(cd, nullptr, nullptr, &op, &ol);
  LIBC_NAMESPACE::iconv_close(cd);
  if (result == static_cast<size_t>(-1))
    return -1;
  return op - out;
}

void expect_utf8(const char *from, const char *in, size_t inlen,
                 const char *utf8, size_t length) {
  char out[16] = {};
  ASSERT_EQ(convert_and_finish("UTF-8", from, in, inlen, out, sizeof(out)),
            static_cast<ssize_t>(length));
  for (size_t i = 0; i < length; ++i)
    EXPECT_EQ(out[i], utf8[i]);
}

TEST_F(LlvmLibcIconvTest, ReadingJoinsALetterAndAMark) {
  // a and a combining grave accent.
  expect_utf8("CP1258", "a\xcc", 2, "\xc3\xa0", 2);
  // a and a combining dot below, which CP1258 has no byte for together.
  expect_utf8("CP1258", "a\xf2", 2, "\xe1\xba\xa1", 3);
  expect_utf8("TCVN", "a\xb3", 2, "\xc3\xa1", 2);
  // A letter already joined may take another mark: O with a tilde and then an
  // acute.
  expect_utf8("CP1258", "O\xde\xec", 3, "\xe1\xb9\x8c", 3);
  // The same marks the other way round are other text, so stay apart.
  expect_utf8("CP1258", "\xd3\xde", 2, "\xc3\x93\xcc\x83", 4);
  // Hebrew shin with a dagesh and a shin dot, in either order.
  expect_utf8("CP1255", "\xf9\xcc\xd1", 3, "\xef\xac\xac", 3);
  expect_utf8("CP1255", "\xf9\xd1\xcc", 3, "\xef\xac\xac", 3);
  // A mark which does not join the letter before it, or has none, is read as
  // it is.
  expect_utf8("CP1258", "b\xcc", 2, "b\xcc\x80", 3);
  expect_utf8("CP1258", "\xcc", 1, "\xcc\x80", 2);
}

TEST_F(LlvmLibcIconvTest, WritingSplitsWhatASetHasNoByteFor) {
  // a with a dot below: a letter and a mark in CP1258, one byte in TCVN.
  expect_bytes("CP1258", "UTF-8", "\xe1\xba\xa1", 3, "a\xf2", 2);
  expect_bytes("TCVN", "UTF-8", "\xe1\xba\xa1", 3, "\xb9", 1);
  // O with a tilde and an acute. TCVN has O with a tilde; CP1258 has only O.
  expect_bytes("TCVN", "UTF-8", "\xe1\xb9\x8c", 3, "\x94\xb3", 2);
  expect_bytes("CP1258", "UTF-8", "\xe1\xb9\x8c", 3, "O\xde\xec", 3);
  // Marks come out in canonical order.
  expect_bytes("CP1255", "UTF-8", "\xef\xac\xac", 3, "\xf9\xcc\xd1", 3);

  // With room for the letter but not the mark, nothing is written.
  char out[1] = {};
  EXPECT_EQ(convert("CP1258", "UTF-8", "\xe1\xba\xa1", 3, out, sizeof(out)),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(E2BIG);
  // CP1258 has no mark to make O with a tilde and a diaeresis from.
  char more[8] = {};
  EXPECT_EQ(convert("CP1258", "UTF-8", "\xe1\xb9\x8e", 3, more, sizeof(more)),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(EILSEQ);
}

TEST_F(LlvmLibcIconvTest, ALetterWaitsForAMark) {
  iconv_t cd = LIBC_NAMESPACE::iconv_open("UTF-8", "CP1258");
  ASSERT_TRUE(cd != FAILED);
  char input[] = "a1";
  char out[8] = {};
  char *ip = input;
  size_t il = 1;
  char *op = out;
  size_t ol = sizeof(out);
  // A mark may still join a, so it is used but not yet written.
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, &ip, &il, &op, &ol), size_t(0));
  EXPECT_EQ(il, size_t(0));
  EXPECT_EQ(static_cast<long>(op - out), 0L);
  // Going back to the initial state with somewhere to write it writes it.
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, nullptr, nullptr, &op, &ol), size_t(0));
  EXPECT_EQ(static_cast<long>(op - out), 1L);
  EXPECT_EQ(out[0], 'a');

  // Without, it is dropped.
  ip = input;
  il = 1;
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, &ip, &il, &op, &ol), size_t(0));
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, nullptr, nullptr, nullptr, nullptr),
            size_t(0));
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, nullptr, nullptr, &op, &ol), size_t(0));
  EXPECT_EQ(static_cast<long>(op - out), 1L);

  // No mark joins 1, so it is written at once.
  il = 1;
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, &ip, &il, &op, &ol), size_t(0));
  EXPECT_EQ(static_cast<long>(op - out), 2L);
  EXPECT_EQ(out[1], '1');
  ASSERT_EQ(LIBC_NAMESPACE::iconv_close(cd), 0);
}

TEST_F(LlvmLibcIconvTest, AnErrorHandsBackAHeldLetter) {
  iconv_t cd = LIBC_NAMESPACE::iconv_open("UTF-32BE", "CP1258");
  ASSERT_TRUE(cd != FAILED);
  char input[] = "abc";
  char out[8] = {};
  char *ip = input;
  size_t il = 3;
  char *op = out;
  size_t ol = 4;
  // b shows that no mark joins a, so a is written and b held. c would write b,
  // which there is no room for.
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, &ip, &il, &op, &ol),
            static_cast<size_t>(-1));
  ASSERT_ERRNO_EQ(E2BIG);
  EXPECT_EQ(il, size_t(1));
  EXPECT_EQ(static_cast<long>(op - out), 4L);
  EXPECT_EQ(out[3], 'a');
  ol = 4;
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, nullptr, nullptr, &op, &ol), size_t(0));
  EXPECT_EQ(out[7], 'b');

  // Nothing is written after a, so the error hands a back as well.
  ip = input;
  il = 2;
  op = out;
  ol = 1;
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, &ip, &il, &op, &ol),
            static_cast<size_t>(-1));
  ASSERT_ERRNO_EQ(E2BIG);
  EXPECT_EQ(il, size_t(2));
  ol = sizeof(out);
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, nullptr, nullptr, &op, &ol), size_t(0));
  EXPECT_EQ(static_cast<long>(op - out), 0L);
  ASSERT_EQ(LIBC_NAMESPACE::iconv_close(cd), 0);

  // The same when the letter has no place in the target set: A with a breve
  // in ASCII.
  const char breve[] = {'\xc3', 'a'};
  Result r = convert_all_from("ASCII", "CP1258", breve, 2, out, sizeof(out));
  EXPECT_EQ(r.error, EILSEQ);
  EXPECT_EQ(r.used, size_t(0));
  // Under //IGNORE it is left out, and the letter after it waits for a mark.
  r = convert_all_from("ASCII//IGNORE", "CP1258", breve, 2, out, sizeof(out));
  EXPECT_EQ(r.error, EILSEQ);
  EXPECT_EQ(r.used, size_t(2));
  EXPECT_EQ(r.made, size_t(0));
}

TEST_F(LlvmLibcIconvTest, ACodeForSeveralCharacters) {
  // A Yiddish ligature, which Apple gives as yod yod and a patah.
  expect_utf8("MacHebrew", "\x81", 1, "\xd7\xb2\xd6\xb7", 4);
  // Lamed and holam, grouped by one of Apple's transcoding hints.
  expect_utf8("MacHebrew", "\xc0", 1, "\xef\xa1\xaa\xd7\x9c\xd6\xb9", 7);
  // Mai ek, with a hint to draw it low and to the left.
  expect_utf8("MacThai", "\x83", 1, "\xe0\xb9\x88\xef\xa1\xb5", 6);
  // A plus sign for right to left text is still a plus sign.
  expect_utf8("MacHebrew", "\xab", 1, "+", 1);

  // Writing waits to see whether a character begins a sequence.
  expect_bytes("MacHebrew", "UTF-8", "\xd7\xb2\xd6\xb7", 4, "\x81", 1);
  expect_bytes("MacHebrew", "UTF-8", "\xd6\xb8\xef\xa1\xbf", 5, "\xde", 1);
  const char mai_ek_a[] = {'\xe0', '\xb9', '\x88', 'a'};
  const char written[] = {'\xe8', 'a'};
  expect_bytes("MacThai", "UTF-8", mai_ek_a, 4, written, 2);
  // One held back at the end is written as it is.
  char out[8] = {};
  ASSERT_EQ(
      convert_and_finish("MacThai", "UTF-8", mai_ek_a, 3, out, sizeof(out)),
      ssize_t(1));
  EXPECT_EQ(out[0], '\xe8');
}

TEST_F(LlvmLibcIconvTest, ErrorsPartWayThroughASequence) {
  // Yod yod is held back for a patah. What comes instead leaves it to be
  // written alone, and MacHebrew has no byte for it.
  char out[8] = {};
  const char yod_yod_a[] = {'\xd7', '\xb2', 'a'};
  Result r = convert_all("MacHebrew", yod_yod_a, 3, out, sizeof(out));
  EXPECT_EQ(r.error, EILSEQ);
  EXPECT_EQ(r.used, size_t(2));
  EXPECT_EQ(r.made, size_t(0));

  // With room for only the first character of a code, the code is used and
  // the rest is owed.
  iconv_t cd = LIBC_NAMESPACE::iconv_open("UTF-32BE", "MacHebrew");
  ASSERT_TRUE(cd != FAILED);
  char input[] = {'\x81', 'A'};
  char wide[8] = {};
  char *ip = input;
  size_t il = 1;
  char *op = wide;
  size_t ol = 4;
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, &ip, &il, &op, &ol),
            static_cast<size_t>(-1));
  ASSERT_ERRNO_EQ(E2BIG);
  EXPECT_EQ(il, size_t(0));
  EXPECT_EQ(wide[3], '\xf2');
  // What is owed goes out before anything more is read.
  il = 1;
  ol = 4;
  EXPECT_EQ(LIBC_NAMESPACE::iconv(cd, &ip, &il, &op, &ol),
            static_cast<size_t>(-1));
  ASSERT_ERRNO_EQ(E2BIG);
  EXPECT_EQ(il, size_t(1));
  EXPECT_EQ(wide[7], '\xb7');
  ASSERT_EQ(LIBC_NAMESPACE::iconv_close(cd), 0);
}

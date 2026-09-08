//===-- Unittests for mbrtoc32 and c32rtomb -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/char32_t.h"
#include "hdr/types/mbstate_t.h"
#include "src/__support/libc_errno.h"
#include "src/uchar/c32rtomb.h"
#include "src/uchar/mbrtoc32.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcUcharTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcUcharTest, DecodesAscii) {
  mbstate_t state{};
  char32_t c32 = 0;
  ASSERT_EQ(LIBC_NAMESPACE::mbrtoc32(&c32, "A", 1, &state), size_t(1));
  EXPECT_EQ(static_cast<unsigned>(c32), 0x41u);
}

TEST_F(LlvmLibcUcharTest, DecodesMultibyte) {
  mbstate_t state{};
  char32_t c32 = 0;
  // U+00E9, two bytes.
  ASSERT_EQ(LIBC_NAMESPACE::mbrtoc32(&c32, "\xc3\xa9", 2, &state), size_t(2));
  EXPECT_EQ(static_cast<unsigned>(c32), 0xE9u);

  // U+4E00, three bytes.
  ASSERT_EQ(LIBC_NAMESPACE::mbrtoc32(&c32, "\xe4\xb8\x80", 3, &state),
            size_t(3));
  EXPECT_EQ(static_cast<unsigned>(c32), 0x4E00u);

  // U+1F600, four bytes.
  ASSERT_EQ(LIBC_NAMESPACE::mbrtoc32(&c32, "\xf0\x9f\x98\x80", 4, &state),
            size_t(4));
  EXPECT_EQ(static_cast<unsigned>(c32), 0x1F600u);
}

TEST_F(LlvmLibcUcharTest, NullCharacterReportsZero) {
  mbstate_t state{};
  char32_t c32 = 0xFFFF;
  EXPECT_EQ(LIBC_NAMESPACE::mbrtoc32(&c32, "\0", 1, &state), size_t(0));
  EXPECT_EQ(static_cast<unsigned>(c32), 0u);
}

TEST_F(LlvmLibcUcharTest, IncompleteSequence) {
  mbstate_t state{};
  char32_t c32 = 0;
  // The first two bytes of a three byte character.
  EXPECT_EQ(LIBC_NAMESPACE::mbrtoc32(&c32, "\xe4\xb8", 2, &state),
            static_cast<size_t>(-2));
}

TEST_F(LlvmLibcUcharTest, InvalidSequence) {
  mbstate_t state{};
  char32_t c32 = 0;
  // A continuation byte with nothing to continue.
  EXPECT_EQ(LIBC_NAMESPACE::mbrtoc32(&c32, "\x80", 1, &state),
            static_cast<size_t>(-1));
  ASSERT_ERRNO_EQ(EILSEQ);
}

TEST_F(LlvmLibcUcharTest, Encodes) {
  mbstate_t state{};
  char buf[8] = {};
  EXPECT_EQ(LIBC_NAMESPACE::c32rtomb(buf, U'A', &state), size_t(1));
  EXPECT_EQ(static_cast<unsigned>(static_cast<unsigned char>(buf[0])), 0x41u);

  EXPECT_EQ(LIBC_NAMESPACE::c32rtomb(buf, char32_t(0x00E9), &state), size_t(2));
  EXPECT_EQ(static_cast<unsigned>(static_cast<unsigned char>(buf[0])), 0xC3u);
  EXPECT_EQ(static_cast<unsigned>(static_cast<unsigned char>(buf[1])), 0xA9u);

  EXPECT_EQ(LIBC_NAMESPACE::c32rtomb(buf, char32_t(0x1F600), &state),
            size_t(4));
  EXPECT_EQ(static_cast<unsigned>(static_cast<unsigned char>(buf[0])), 0xF0u);
}

TEST_F(LlvmLibcUcharTest, EncodeRoundTrips) {
  mbstate_t state{};
  char buf[8] = {};
  const char32_t cases[] = {char32_t(0x41), char32_t(0x00E9), char32_t(0x4E00),
                            char32_t(0x1F600)};
  for (char32_t original : cases) {
    size_t written = LIBC_NAMESPACE::c32rtomb(buf, original, &state);
    ASSERT_NE(written, static_cast<size_t>(-1));
    char32_t back = 0;
    mbstate_t read_state{};
    ASSERT_EQ(LIBC_NAMESPACE::mbrtoc32(&back, buf, written, &read_state),
              written);
    EXPECT_EQ(static_cast<unsigned>(back), static_cast<unsigned>(original));
  }
}

TEST_F(LlvmLibcUcharTest, NullBufferAsksAboutTheNullCharacter) {
  mbstate_t state{};
  EXPECT_EQ(LIBC_NAMESPACE::c32rtomb(nullptr, U'A', &state), size_t(1));
}

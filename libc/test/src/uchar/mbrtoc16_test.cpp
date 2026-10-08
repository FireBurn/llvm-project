//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for mbrtoc16 and c16rtomb
///
//===----------------------------------------------------------------------===//

#include "hdr/types/char16_t.h"
#include "hdr/types/mbstate_t.h"
#include "src/uchar/c16rtomb.h"
#include "src/uchar/mbrtoc16.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

static unsigned byte(char c) { return static_cast<unsigned char>(c); }

using LlvmLibcMbrtoc16Test = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcMbrtoc16Test, DecodesBasicMultilingualPlane) {
  mbstate_t state{};
  char16_t c16 = 0;
  ASSERT_EQ(LIBC_NAMESPACE::mbrtoc16(&c16, "A", 1, &state), size_t(1));
  EXPECT_EQ(static_cast<unsigned>(c16), 0x41u);
  ASSERT_EQ(LIBC_NAMESPACE::mbrtoc16(&c16, "\xc3\xa9", 2, &state), size_t(2));
  EXPECT_EQ(static_cast<unsigned>(c16), 0xE9u);
  ASSERT_EQ(LIBC_NAMESPACE::mbrtoc16(&c16, "\xe4\xb8\x80", 3, &state),
            size_t(3));
  EXPECT_EQ(static_cast<unsigned>(c16), 0x4E00u);
}

// U+1F600 is the pair D83D DE00. The second unit is returned by a call that
// consumes no input, which reports (size_t)-3.
TEST_F(LlvmLibcMbrtoc16Test, DecodesASurrogatePair) {
  mbstate_t state{};
  char16_t c16 = 0;
  ASSERT_EQ(LIBC_NAMESPACE::mbrtoc16(&c16, "\xf0\x9f\x98\x80", 4, &state),
            size_t(4));
  EXPECT_EQ(static_cast<unsigned>(c16), 0xD83Du);
  ASSERT_EQ(LIBC_NAMESPACE::mbrtoc16(&c16, "", 1, &state), size_t(-3));
  EXPECT_EQ(static_cast<unsigned>(c16), 0xDE00u);
}

TEST_F(LlvmLibcMbrtoc16Test, NullCharacterReportsZero) {
  mbstate_t state{};
  char16_t c16 = 0xFFFF;
  EXPECT_EQ(LIBC_NAMESPACE::mbrtoc16(&c16, "\0", 1, &state), size_t(0));
  EXPECT_EQ(static_cast<unsigned>(c16), 0u);
}

TEST_F(LlvmLibcMbrtoc16Test, InvalidSequenceSetsEILSEQ) {
  mbstate_t state{};
  char16_t c16 = 0;
  EXPECT_EQ(LIBC_NAMESPACE::mbrtoc16(&c16, "\xff", 1, &state), size_t(-1));
  ASSERT_ERRNO_EQ(EILSEQ);
}

TEST_F(LlvmLibcMbrtoc16Test, EncodesBasicMultilingualPlane) {
  mbstate_t state{};
  char buf[4] = {};
  ASSERT_EQ(LIBC_NAMESPACE::c16rtomb(buf, char16_t(0x41), &state), size_t(1));
  EXPECT_EQ(buf[0], 'A');
  ASSERT_EQ(LIBC_NAMESPACE::c16rtomb(buf, char16_t(0x4E00), &state), size_t(3));
  EXPECT_EQ(byte(buf[0]), 0xE4u);
  EXPECT_EQ(byte(buf[1]), 0xB8u);
  EXPECT_EQ(byte(buf[2]), 0x80u);
}

TEST_F(LlvmLibcMbrtoc16Test, EncodesASurrogatePairAsOneCharacter) {
  mbstate_t state{};
  char buf[4] = {};
  // The high half produces nothing until the low half arrives.
  ASSERT_EQ(LIBC_NAMESPACE::c16rtomb(buf, char16_t(0xD83D), &state), size_t(0));
  ASSERT_EQ(LIBC_NAMESPACE::c16rtomb(buf, char16_t(0xDE00), &state), size_t(4));
  EXPECT_EQ(byte(buf[0]), 0xF0u);
  EXPECT_EQ(byte(buf[1]), 0x9Fu);
  EXPECT_EQ(byte(buf[2]), 0x98u);
  EXPECT_EQ(byte(buf[3]), 0x80u);
}

TEST_F(LlvmLibcMbrtoc16Test, LoneLowSurrogateIsInvalid) {
  mbstate_t state{};
  char buf[4] = {};
  EXPECT_EQ(LIBC_NAMESPACE::c16rtomb(buf, char16_t(0xDE00), &state),
            size_t(-1));
  ASSERT_ERRNO_EQ(EILSEQ);
}

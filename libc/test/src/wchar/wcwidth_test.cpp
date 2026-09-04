//===-- Unittests for wcwidth and wcswidth --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/wchar_t.h"
#include "src/wchar/wcswidth.h"
#include "src/wchar/wcwidth.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcWcwidthTest, Ascii) {
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(L'A'), 1);
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(L' '), 1);
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(L'~'), 1);
}

TEST(LlvmLibcWcwidthTest, NulIsZeroAndControlsAreNotPrintable) {
  // A null wide character takes no room, but is not the same as a control
  // character, which cannot be displayed at all.
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(L'\0'), 0);
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x07), -1); // BEL
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(L'\n'), -1);
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x7F), -1);   // DEL
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x9F), -1);   // end of the C1 range
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x2028), -1); // line separator
}

TEST(LlvmLibcWcwidthTest, CombiningMarksTakeNoRoom) {
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x0301), 0); // combining acute
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x0591), 0); // hebrew accent etnahta
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x20DD), 0); // combining enclosing circle
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x200B), 0); // zero width space
}

TEST(LlvmLibcWcwidthTest, WideCharactersTakeTwoColumns) {
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x4E00), 2);  // CJK ideograph
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x3042), 2);  // hiragana A
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0xFF21), 2);  // fullwidth A
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0xAC00), 2);  // hangul syllable
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x1F600), 2); // emoji
}

TEST(LlvmLibcWcwidthTest, NarrowNonAscii) {
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x00E9), 1); // e with acute
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x0416), 1); // cyrillic zhe
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x05D0), 1); // hebrew alef
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x00AD), 1); // soft hyphen is shown
}

TEST(LlvmLibcWcwidthTest, HangulJamoAndFillers) {
  // Conjoining jamo attach to the syllable before them.
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x1160), 0);
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x11FF), 0);
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x3164), 0);
}

TEST(LlvmLibcWcwidthTest, OutOfRange) {
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x110000), -1);
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0xD800), -1); // a lone surrogate
}

TEST(LlvmLibcWcwidthTest, UnassignedIsOneColumn) {
  // A code point these tables do not know is taken as one column so that a
  // character assigned later still displays. glibc reports -1 here.
  EXPECT_EQ(LIBC_NAMESPACE::wcwidth(0x0EE0), 1);
}

TEST(LlvmLibcWcswidthTest, SumsTheString) {
  const wchar_t ascii[] = {L'a', L'b', L'c', L'\0'};
  EXPECT_EQ(LIBC_NAMESPACE::wcswidth(ascii, 3), 3);
  EXPECT_EQ(LIBC_NAMESPACE::wcswidth(ascii, 100), 3);
  EXPECT_EQ(LIBC_NAMESPACE::wcswidth(ascii, 2), 2);
  EXPECT_EQ(LIBC_NAMESPACE::wcswidth(ascii, 0), 0);

  // A combining mark adds nothing and a CJK ideograph adds two.
  const wchar_t mixed[] = {L'e', 0x0301, 0x4E00, L'\0'};
  EXPECT_EQ(LIBC_NAMESPACE::wcswidth(mixed, 3), 3);
}

TEST(LlvmLibcWcswidthTest, OneUnprintableSpoilsTheWhole) {
  const wchar_t with_control[] = {L'a', 0x07, L'b', L'\0'};
  EXPECT_EQ(LIBC_NAMESPACE::wcswidth(with_control, 3), -1);
  // Stopping before it leaves a measurable string.
  EXPECT_EQ(LIBC_NAMESPACE::wcswidth(with_control, 1), 1);
  EXPECT_EQ(LIBC_NAMESPACE::wcswidth(nullptr, 1), -1);
}

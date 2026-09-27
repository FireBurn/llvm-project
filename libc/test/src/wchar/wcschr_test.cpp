//===-- Unittests for wcschr ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/wchar_t.h"
#include "src/wchar/wcschr.h"
#include "test/UnitTest/Test.h"

namespace {
// The function returns wchar_t *, as glibc's does; the tests compare it with
// pointers into const strings.
template <typename... Args> const wchar_t *wcschr_const(Args... args) {
  return LIBC_NAMESPACE::wcschr(args...);
}
} // namespace

TEST(LlvmLibcWCSChrTest, FindsFirstCharacter) {
  // Should return pointer to original string since 'a' is the first character.
  const wchar_t *src = L"abcde";
  ASSERT_EQ(wcschr_const(src, L'a'), src);
}

TEST(LlvmLibcWCSChrTest, FindsMiddleCharacter) {
  // Should return pointer to 'c'.
  const wchar_t *src = L"abcde";
  ASSERT_EQ(wcschr_const(src, L'c'), (src + 2));
}

TEST(LlvmLibcWCSChrTest, FindsLastCharacterThatIsNotNullTerminator) {
  // Should return pointer to 'e'.
  const wchar_t *src = L"abcde";
  ASSERT_EQ(wcschr_const(src, L'e'), (src + 4));
}

TEST(LlvmLibcWCSChrTest, FindsNullTerminator) {
  // Should return pointer to null terminator.
  const wchar_t *src = L"abcde";
  ASSERT_EQ(wcschr_const(src, L'\0'), (src + 5));
}

TEST(LlvmLibcWCSChrTest, CharacterNotWithinStringShouldReturnNullptr) {
  // Since 'z' is not within the string, should return nullptr.
  const wchar_t *src = L"abcde";
  ASSERT_EQ(wcschr_const(src, L'z'), nullptr);
}

TEST(LlvmLibcWCSChrTest, ShouldFindFirstOfDuplicates) {
  // Should return pointer to the first '1'.
  const wchar_t *src = L"abc1def1ghi";
  ASSERT_EQ((int)(wcschr_const(src, L'1') - src), 3);

  // Should return original string since 'X' is the first character.
  const wchar_t *dups = L"XXXXX";
  ASSERT_EQ(wcschr_const(dups, L'X'), dups);
}

TEST(LlvmLibcWCSChrTest, EmptyStringShouldOnlyMatchNullTerminator) {
  // Null terminator should match
  const wchar_t *src = L"";
  ASSERT_EQ(src, wcschr_const(src, L'\0'));
  // All other characters should not match
  ASSERT_EQ(wcschr_const(src, L'Z'), nullptr);
  ASSERT_EQ(wcschr_const(src, L'3'), nullptr);
  ASSERT_EQ(wcschr_const(src, L'*'), nullptr);
}

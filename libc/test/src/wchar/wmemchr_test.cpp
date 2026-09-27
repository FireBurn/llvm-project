//===-- Unittests for wmemchr ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/wchar_t.h"
#include "src/wchar/wmemchr.h"
#include "test/UnitTest/Test.h"

namespace {
// The function returns wchar_t *, as glibc's does; the tests compare it with
// pointers into const strings.
template <typename... Args> const wchar_t *wmemchr_const(Args... args) {
  return LIBC_NAMESPACE::wmemchr(args...);
}
} // namespace

TEST(LlvmLibcWMemChrTest, FindsCharacterAfterNullTerminator) {
  // wmemchr should continue searching after a null terminator.
  const size_t size = 5;
  const wchar_t src[size] = {L'a', L'\0', L'b', L'c', L'\0'};
  ASSERT_EQ(wmemchr_const(src, L'b', size), (src + 2));
}

TEST(LlvmLibcWMemChrTest, FindsCharacterInNonNullTerminatedCollection) {
  const size_t size = 3;
  const wchar_t src[size] = {L'a', L'b', L'c'};
  ASSERT_EQ(wmemchr_const(src, L'b', size), (src + 1));
}

TEST(LlvmLibcWMemChrTest, FindsFirstCharacter) {
  const size_t size = 6;
  const wchar_t *src = L"abcde";
  // Should return original array since 'a' is the first character.
  ASSERT_EQ(wmemchr_const(src, L'a', size), (src));
}

TEST(LlvmLibcWMemChrTest, FindsMiddleCharacter) {
  const size_t size = 6;
  const wchar_t *src = L"abcde";
  // Should return characters after and including 'c'.
  ASSERT_EQ(wmemchr_const(src, L'c', size), (src + 2));
}

TEST(LlvmLibcWMemChrTest, FindsLastCharacterThatIsNotNullTerminator) {
  const size_t size = 6;
  const wchar_t *src = L"abcde";
  // Should return 'e' and null terminator.
  ASSERT_EQ(wmemchr_const(src, L'e', size), (src + 4));
}

TEST(LlvmLibcWMemChrTest, FindsNullTerminator) {
  const size_t size = 6;
  const wchar_t *src = L"abcde";
  // Should return null terminator.
  ASSERT_EQ(wmemchr_const(src, L'\0', size), (src + 5));
}

TEST(LlvmLibcWMemChrTest, CharacterNotWithinStringShouldReturnNullptr) {
  const size_t size = 6;
  const wchar_t *src = L"abcde";
  // Should return nullptr.
  ASSERT_EQ(wmemchr_const(src, L'z', size), nullptr);
}

TEST(LlvmLibcWMemChrTest, CharacterNotWithinSizeShouldReturnNullptr) {
  const size_t size = 3;
  const wchar_t *src = L"abcde";
  // Should return nullptr.
  ASSERT_EQ(wmemchr_const(src, L'd', size), nullptr);
}

TEST(LlvmLibcWMemChrTest, TheSourceShouldNotChange) {
  const size_t size = 3;
  const wchar_t *src = L"ab";
  ASSERT_EQ(wmemchr_const(src, L'a', size), src);
  ASSERT_TRUE(src[0] == L'a');
  ASSERT_TRUE(src[1] == L'b');
  ASSERT_EQ(wmemchr_const(src, L'c', size), nullptr);
  ASSERT_TRUE(src[0] == L'a');
  ASSERT_TRUE(src[1] == L'b');
}

TEST(LlvmLibcWMemChrTest, EmptyStringShouldOnlyMatchNullTerminator) {
  const size_t size = 1;
  const wchar_t *src = L"";
  ASSERT_EQ(wmemchr_const(src, L'\0', size), src);
  ASSERT_EQ(wmemchr_const(src, L'c', size), nullptr);
  ASSERT_EQ(wmemchr_const(src, L'1', size), nullptr);
  ASSERT_EQ(wmemchr_const(src, L'?', size), nullptr);
}

TEST(LlvmLibcWMemChrTest, SingleRepeatedCharacterShouldReturnFirst) {
  const size_t size = 6;
  const wchar_t *src = L"XXXXX";
  ASSERT_EQ(wmemchr_const(src, L'X', size), src);
}

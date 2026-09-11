//===-- Unittests for wcscasecmp ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/wchar/wcscasecmp.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcWcscasecmpTest, EmptyStringsAreEqual) {
  ASSERT_EQ(LIBC_NAMESPACE::wcscasecmp(L"", L""), 0);
}

TEST(LlvmLibcWcscasecmpTest, CaseIsIgnored) {
  ASSERT_EQ(LIBC_NAMESPACE::wcscasecmp(L"aBcD", L"AbCd"), 0);
  ASSERT_EQ(LIBC_NAMESPACE::wcscasecmp(L"AbCd", L"aBcD"), 0);
}

// The difference is taken between the lowered characters: a plain comparison
// puts 'a' after 'B', and this one puts it before.
TEST(LlvmLibcWcscasecmpTest, TheLoweredCharactersAreCompared) {
  ASSERT_LT(LIBC_NAMESPACE::wcscasecmp(L"a", L"B"), 0);
  ASSERT_GT(LIBC_NAMESPACE::wcscasecmp(L"B", L"a"), 0);
}

TEST(LlvmLibcWcscasecmpTest, AShorterStringComesFirst) {
  ASSERT_LT(LIBC_NAMESPACE::wcscasecmp(L"ab", L"ABC"), 0);
  ASSERT_GT(LIBC_NAMESPACE::wcscasecmp(L"ABC", L"ab"), 0);
}

TEST(LlvmLibcWcscasecmpTest, CharactersWithoutCaseAreComparedAsTheyAre) {
  ASSERT_EQ(LIBC_NAMESPACE::wcscasecmp(L"a1_b", L"A1_B"), 0);
  ASSERT_LT(LIBC_NAMESPACE::wcscasecmp(L"a1", L"A2"), 0);
}

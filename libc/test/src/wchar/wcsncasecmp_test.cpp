//===-- Unittests for wcsncasecmp -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/wchar/wcsncasecmp.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcWcsncasecmpTest, NothingToCompareIsEqual) {
  ASSERT_EQ(LIBC_NAMESPACE::wcsncasecmp(L"a", L"b", 0), 0);
}

TEST(LlvmLibcWcsncasecmpTest, CaseIsIgnored) {
  ASSERT_EQ(LIBC_NAMESPACE::wcsncasecmp(L"aBcD", L"AbCd", 4), 0);
}

TEST(LlvmLibcWcsncasecmpTest, OnlyTheFirstNCharactersCount) {
  ASSERT_EQ(LIBC_NAMESPACE::wcsncasecmp(L"abcX", L"ABCy", 3), 0);
  ASSERT_LT(LIBC_NAMESPACE::wcsncasecmp(L"abcX", L"ABCy", 4), 0);
}

// A limit longer than either string stops at the end of the shorter one.
TEST(LlvmLibcWcsncasecmpTest, TheEndOfAStringStopsTheComparison) {
  ASSERT_EQ(LIBC_NAMESPACE::wcsncasecmp(L"ab", L"AB", 10), 0);
  ASSERT_LT(LIBC_NAMESPACE::wcsncasecmp(L"ab", L"ABC", 10), 0);
}

TEST(LlvmLibcWcsncasecmpTest, TheLoweredCharactersAreCompared) {
  ASSERT_LT(LIBC_NAMESPACE::wcsncasecmp(L"a", L"B", 1), 0);
  ASSERT_GT(LIBC_NAMESPACE::wcsncasecmp(L"B", L"a", 1), 0);
}

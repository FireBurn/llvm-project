//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for swscanf
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/stdio_macros.h" // For EOF
#include "hdr/types/wchar_t.h"
#include "src/wchar/swscanf.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"
#include "test/UnitTest/Utf8Locale.h"

using LlvmLibcSwscanfTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcSwscanfTest, Numbers) {
  int i = 0;
  double d = 0;
  int n = 0;
  ASSERT_EQ(
      LIBC_NAMESPACE::swscanf(L"  42 -1.5e1 tail", L"%d %lf%n", &i, &d, &n), 2);
  ASSERT_EQ(i, 42);
  ASSERT_TRUE(d == -15.0);
  ASSERT_EQ(n, 11);
}

TEST_F(LlvmLibcSwscanfTest, Strings) {
  wchar_t wide[16];
  char narrow[16];
  ASSERT_EQ(LIBC_NAMESPACE::swscanf(L"word \u00e9x", L"%ls %s", wide, narrow),
            2);
  ASSERT_EQ(wide[0], L'w');
  ASSERT_EQ(wide[3], L'd');
  ASSERT_EQ(wide[4], L'\0');
  ASSERT_EQ(narrow[0], '\xc3');
  ASSERT_EQ(narrow[1], '\xa9');
  ASSERT_EQ(narrow[2], 'x');
}

TEST_F(LlvmLibcSwscanfTest, EndOfInput) {
  int i = 0;
  ASSERT_EQ(LIBC_NAMESPACE::swscanf(L"", L"%d", &i), EOF);
  ASSERT_EQ(LIBC_NAMESPACE::swscanf(L"x", L"%d", &i), 0);
}

TEST_F(LlvmLibcSwscanfTest, NonAsciiFormatIsRefused) {
  int i = 0;
  ASSERT_EQ(LIBC_NAMESPACE::swscanf(L"1", L"\u00e9%d", &i), EOF);
  ASSERT_ERRNO_EQ(EILSEQ);
}

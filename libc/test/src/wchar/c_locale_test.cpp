//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the multibyte conversions in the C locale
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/locale_macros.h"
#include "hdr/types/mbstate_t.h"
#include "hdr/types/wchar_t.h"
#include "src/locale/setlocale.h"
#include "src/wchar/mbrtowc.h"
#include "src/wchar/wcrtomb.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcCLocaleTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// The program starts in the C locale, which is ASCII. Only UTF-8 locales
// convert anything else, and a program has to ask for one.
TEST_F(LlvmLibcCLocaleTest, OnlyAsciiConverts) {
  mbstate_t state{};
  wchar_t wc = 0;
  ASSERT_EQ(LIBC_NAMESPACE::mbrtowc(&wc, "a", 1, &state), size_t(1));
  ASSERT_EQ(wc, wchar_t(L'a'));

  // U+00E9 as UTF-8.
  ASSERT_EQ(
      static_cast<int>(LIBC_NAMESPACE::mbrtowc(&wc, "\xc3\xa9", 2, &state)),
      -1);
  ASSERT_ERRNO_EQ(EILSEQ);
  LIBC_NAMESPACE::libc_errno = 0;

  state = mbstate_t{};
  char out[8];
  ASSERT_EQ(LIBC_NAMESPACE::wcrtomb(out, L'a', &state), size_t(1));
  ASSERT_EQ(
      static_cast<int>(LIBC_NAMESPACE::wcrtomb(out, wchar_t(0xe9), &state)),
      -1);
  ASSERT_ERRNO_EQ(EILSEQ);
  LIBC_NAMESPACE::libc_errno = 0;
}

TEST_F(LlvmLibcCLocaleTest, AUtf8LocaleConvertsTheRest) {
  ASSERT_TRUE(LIBC_NAMESPACE::setlocale(LC_CTYPE, "C.UTF-8") != nullptr);
  mbstate_t state{};
  wchar_t wc = 0;
  ASSERT_EQ(LIBC_NAMESPACE::mbrtowc(&wc, "\xc3\xa9", 2, &state), size_t(2));
  ASSERT_EQ(wc, wchar_t(0xe9));

  char out[8];
  state = mbstate_t{};
  ASSERT_EQ(LIBC_NAMESPACE::wcrtomb(out, wchar_t(0xe9), &state), size_t(2));
  LIBC_NAMESPACE::setlocale(LC_CTYPE, "C");
}

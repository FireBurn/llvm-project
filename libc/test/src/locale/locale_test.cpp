//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for locale.
///
//===----------------------------------------------------------------------===//

#include "hdr/locale_macros.h"
#include "src/locale/freelocale.h"
#include "src/locale/newlocale.h"
#include "src/locale/setlocale.h"
#include "src/locale/uselocale.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcLocale, DefaultLocale) {
  locale_t new_locale = LIBC_NAMESPACE::newlocale(LC_ALL_MASK, "C", nullptr);
  EXPECT_NE(new_locale, static_cast<locale_t>(nullptr));

  locale_t old_locale = LIBC_NAMESPACE::uselocale(new_locale);
  EXPECT_NE(old_locale, static_cast<locale_t>(nullptr));

  LIBC_NAMESPACE::freelocale(new_locale);

  LIBC_NAMESPACE::uselocale(old_locale);
}

TEST(LlvmLibcLocale, NewLocaleValidation) {
  // Choosing masks within LC_*_MASK is OK.
  locale_t loc =
      LIBC_NAMESPACE::newlocale(LC_CTYPE_MASK | LC_NUMERIC_MASK, "C", nullptr);
  EXPECT_NE(loc, static_cast<locale_t>(nullptr));
  LIBC_NAMESPACE::freelocale(loc);

  // Empty locale name is implementation-defined,
  // defaults to C locale.
  loc = LIBC_NAMESPACE::newlocale(LC_ALL_MASK, "", nullptr);
  EXPECT_NE(loc, static_cast<locale_t>(nullptr));
  LIBC_NAMESPACE::freelocale(loc);

  // Masks outside the valid range are rejected.
  loc = LIBC_NAMESPACE::newlocale(~0, "C", nullptr);
  EXPECT_EQ(loc, static_cast<locale_t>(nullptr));

  // A name stating a character set that cannot be honoured is rejected,
  // rather than answered with a locale that would encode differently.
  loc = LIBC_NAMESPACE::newlocale(LC_ALL_MASK, "ja_JP.EUC-JP", nullptr);
  EXPECT_EQ(loc, static_cast<locale_t>(nullptr));

  // The names a machine is actually set to are honoured.
  loc = LIBC_NAMESPACE::newlocale(LC_ALL_MASK, "C.UTF-8", nullptr);
  EXPECT_NE(loc, static_cast<locale_t>(nullptr));
  LIBC_NAMESPACE::freelocale(loc);

  loc = LIBC_NAMESPACE::newlocale(LC_ALL_MASK, "en_GB.UTF-8", nullptr);
  EXPECT_NE(loc, static_cast<locale_t>(nullptr));
  LIBC_NAMESPACE::freelocale(loc);
}

TEST(LlvmLibcLocale, SetLocale) {
  // Setting a "C" or default locale.
  EXPECT_STREQ(LIBC_NAMESPACE::setlocale(LC_ALL, "C"), "C");
  // "" selects what the environment names, so only that it is accepted.
  EXPECT_NE(LIBC_NAMESPACE::setlocale(LC_CTYPE, ""),
            static_cast<char *>(nullptr));
  EXPECT_STREQ(LIBC_NAMESPACE::setlocale(LC_ALL, "C"), "C");
  // Querying the current locale.
  EXPECT_STREQ(LIBC_NAMESPACE::setlocale(LC_ALL, nullptr), "C");
  EXPECT_STREQ(LIBC_NAMESPACE::setlocale(LC_NUMERIC, nullptr), "C");

  // Unsupporter categories or non-C locales.
  EXPECT_EQ(LIBC_NAMESPACE::setlocale(LC_ALL + 1, "C"),
            static_cast<char *>(nullptr));
  EXPECT_EQ(LIBC_NAMESPACE::setlocale(LC_ALL + 1, nullptr),
            static_cast<char *>(nullptr));
  EXPECT_EQ(LIBC_NAMESPACE::setlocale(LC_ALL, "ja_JP.EUC-JP"),
            static_cast<char *>(nullptr));
}

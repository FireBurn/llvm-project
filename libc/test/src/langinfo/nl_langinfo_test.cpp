//===-- Unittests for nl_langinfo -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/langinfo_macros.h"
#include "hdr/locale_macros.h"
#include "src/langinfo/nl_langinfo.h"
#include "src/langinfo/nl_langinfo_l.h"
#include "src/locale/locale.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcNlLanginfoTest, Codeset) {
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(CODESET), "ANSI_X3.4-1968");
}

TEST(LlvmLibcNlLanginfoTest, Numeric) {
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(RADIXCHAR), ".");
  // The C locale groups no digits, so the separator is empty.
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(THOUSEP), "");
}

TEST(LlvmLibcNlLanginfoTest, TimeFormats) {
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(D_T_FMT), "%a %b %e %H:%M:%S %Y");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(D_FMT), "%m/%d/%y");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(T_FMT), "%H:%M:%S");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(T_FMT_AMPM), "%I:%M:%S %p");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(AM_STR), "AM");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(PM_STR), "PM");
}

TEST(LlvmLibcNlLanginfoTest, DayAndMonthNames) {
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(DAY_1), "Sunday");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(DAY_7), "Saturday");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(ABDAY_1), "Sun");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(ABDAY_7), "Sat");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(MON_1), "January");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(MON_12), "December");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(ABMON_1), "Jan");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(ABMON_12), "Dec");
}

TEST(LlvmLibcNlLanginfoTest, MessagesAndMonetary) {
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(YESEXPR), "^[yY]");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(NOEXPR), "^[nN]");
  // The C locale has no currency symbol and no eras.
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(CRNCYSTR), "");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(ERA), "");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(ALT_DIGITS), "");
}

TEST(LlvmLibcNlLanginfoTest, AnItemWhichIsNotThere) {
  // POSIX says an unrecognised item gives an empty string rather than an
  // error.
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(_NL_ITEM(LC_TIME, 9999)), "");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(_NL_ITEM(99, 0)), "");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(-1), "");
}

TEST(LlvmLibcNlLanginfoTest, LocaleFormAgrees) {
  // Only the C locale exists, so the two forms cannot disagree.
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo_l(DAY_1, LIBC_NAMESPACE::locale),
               "Sunday");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo_l(CODESET, LIBC_NAMESPACE::locale),
               LIBC_NAMESPACE::nl_langinfo(CODESET));
}

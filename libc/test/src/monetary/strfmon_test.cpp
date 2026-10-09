//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for strfmon and strfmon_l
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/monetary/strfmon.h"
#include "src/monetary/strfmon_l.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcStrfmonTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// The C locale has no currency symbol and no grouping, and two digits after
// the point, which is all these rely on.

TEST_F(LlvmLibcStrfmonTest, NationalFormat) {
  char buf[64];
  ASSERT_EQ(LIBC_NAMESPACE::strfmon(buf, sizeof(buf), "%n", 1234.567),
            ssize_t(7));
  ASSERT_STREQ(buf, "1234.57");
}

TEST_F(LlvmLibcStrfmonTest, InternationalFormatMatchesInTheCLocale) {
  char buf[64];
  ASSERT_EQ(LIBC_NAMESPACE::strfmon(buf, sizeof(buf), "%i", 1234.567),
            ssize_t(7));
  ASSERT_STREQ(buf, "1234.57");
}

TEST_F(LlvmLibcStrfmonTest, WidthAndLeftJustification) {
  char buf[64];
  ASSERT_EQ(LIBC_NAMESPACE::strfmon(buf, sizeof(buf), "[%10n]", 12.5),
            ssize_t(12));
  ASSERT_STREQ(buf, "[     12.50]");
  ASSERT_EQ(LIBC_NAMESPACE::strfmon(buf, sizeof(buf), "[%-10n]", 12.5),
            ssize_t(12));
  ASSERT_STREQ(buf, "[12.50     ]");
}

TEST_F(LlvmLibcStrfmonTest, PrecisionAndNegativeValues) {
  char buf[64];
  ASSERT_EQ(LIBC_NAMESPACE::strfmon(buf, sizeof(buf), "%.0n", 1234.567),
            ssize_t(4));
  ASSERT_STREQ(buf, "1235");
  ASSERT_EQ(LIBC_NAMESPACE::strfmon(buf, sizeof(buf), "%n", -5.0), ssize_t(5));
  ASSERT_STREQ(buf, "-5.00");
  ASSERT_EQ(LIBC_NAMESPACE::strfmon(buf, sizeof(buf), "%(n", -5.0), ssize_t(6));
  ASSERT_STREQ(buf, "(5.00)");
}

TEST_F(LlvmLibcStrfmonTest, SeveralConversionsAndLiteralText) {
  char buf[64];
  ASSERT_EQ(LIBC_NAMESPACE::strfmon(buf, sizeof(buf), "a %n b %n %%", 1.0, 2.0),
            ssize_t(15));
  ASSERT_STREQ(buf, "a 1.00 b 2.00 %");
}

TEST_F(LlvmLibcStrfmonTest, ResultThatDoesNotFitIsE2BIG) {
  char buf[4];
  ASSERT_EQ(LIBC_NAMESPACE::strfmon(buf, sizeof(buf), "%n", 1234.567),
            ssize_t(-1));
  ASSERT_ERRNO_EQ(E2BIG);
}

TEST_F(LlvmLibcStrfmonTest, LocaleVariantAgreesWithTheCurrentLocale) {
  char buf[64];
  ASSERT_EQ(LIBC_NAMESPACE::strfmon_l(buf, sizeof(buf), nullptr, "%n", 3.14159),
            ssize_t(4));
  ASSERT_STREQ(buf, "3.14");
}

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the tests of the Plural-Forms evaluator.
///
//===----------------------------------------------------------------------===//

#include "src/__support/CPP/string_view.h"
#include "src/__support/libintl/plural.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::libintl::PluralRule;

TEST(LlvmLibcPluralRuleTest, English) {
  PluralRule rule("Plural-Forms: nplurals=2; plural=(n != 1);\n");
  ASSERT_EQ(rule.forms(), 2UL);
  ASSERT_EQ(rule.form(0), 1UL);
  ASSERT_EQ(rule.form(1), 0UL);
  ASSERT_EQ(rule.form(2), 1UL);
}

TEST(LlvmLibcPluralRuleTest, French) {
  PluralRule rule("Content-Type: text/plain\nPlural-Forms: nplurals=2; "
                  "plural=(n > 1);\n");
  ASSERT_EQ(rule.form(0), 0UL);
  ASSERT_EQ(rule.form(1), 0UL);
  ASSERT_EQ(rule.form(2), 1UL);
}

TEST(LlvmLibcPluralRuleTest, NoPlurals) {
  PluralRule rule("Plural-Forms: nplurals=1; plural=0;\n");
  ASSERT_EQ(rule.forms(), 1UL);
  ASSERT_EQ(rule.form(0), 0UL);
  ASSERT_EQ(rule.form(5), 0UL);
}

TEST(LlvmLibcPluralRuleTest, Russian) {
  PluralRule rule(
      "Plural-Forms: nplurals=3; plural=n%10==1 && n%100!=11 ? 0 : "
      "n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2;\n");
  ASSERT_EQ(rule.form(1), 0UL);
  ASSERT_EQ(rule.form(21), 0UL);
  ASSERT_EQ(rule.form(11), 2UL);
  ASSERT_EQ(rule.form(2), 1UL);
  ASSERT_EQ(rule.form(24), 1UL);
  ASSERT_EQ(rule.form(12), 2UL);
  ASSERT_EQ(rule.form(100), 2UL);
}

TEST(LlvmLibcPluralRuleTest, Arabic) {
  PluralRule rule("Plural-Forms: nplurals=6; plural=n==0 ? 0 : n==1 ? 1 : "
                  "n==2 ? 2 : n%100>=3 && n%100<=10 ? 3 : n%100>=11 ? 4 : 5;");
  ASSERT_EQ(rule.form(0), 0UL);
  ASSERT_EQ(rule.form(2), 2UL);
  ASSERT_EQ(rule.form(5), 3UL);
  ASSERT_EQ(rule.form(11), 4UL);
  ASSERT_EQ(rule.form(101), 5UL);
}

TEST(LlvmLibcPluralRuleTest, UnusableRuleFallsBackToEnglish) {
  const char *headers[] = {"",
                           "Content-Type: text/plain\n",
                           "Plural-Forms: nplurals=2;",
                           "Plural-Forms: nplurals=2; plural=(n;",
                           "Plural-Forms: nplurals=2; plural=n/0;",
                           "Plural-Forms: nplurals=0; plural=0;"};
  for (const char *header : headers) {
    PluralRule rule(header);
    ASSERT_EQ(rule.form(1), 0UL);
    ASSERT_EQ(rule.form(3), 1UL);
  }
}

TEST(LlvmLibcPluralRuleTest, FormBeyondTheCountIsRefused) {
  PluralRule rule("Plural-Forms: nplurals=2; plural=n;");
  ASSERT_EQ(rule.form(1), 1UL);
  ASSERT_EQ(rule.form(7), 1UL);
}

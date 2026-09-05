//===-- Unittests for setlocale -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/langinfo_macros.h"
#include "hdr/locale_macros.h"
#include "src/langinfo/nl_langinfo.h"
#include "src/locale/setlocale.h"
#include "src/stdlib/mb_cur_max.h"
#include "src/stdlib/setenv.h"
#include "src/stdlib/unsetenv.h"
#include "test/UnitTest/Test.h"

namespace {

class LlvmLibcSetlocale : public LIBC_NAMESPACE::testing::Test {
public:
  void SetUp() override {
    LIBC_NAMESPACE::unsetenv("LC_ALL");
    LIBC_NAMESPACE::unsetenv("LC_CTYPE");
    LIBC_NAMESPACE::unsetenv("LANG");
    LIBC_NAMESPACE::setlocale(LC_ALL, "C");
  }
};

} // anonymous namespace

TEST_F(LlvmLibcSetlocale, TheCLocale) {
  ASSERT_STREQ(LIBC_NAMESPACE::setlocale(LC_ALL, "C"), "C");
  ASSERT_STREQ(LIBC_NAMESPACE::setlocale(LC_ALL, nullptr), "C");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(CODESET), "ANSI_X3.4-1968");
  ASSERT_EQ(LIBC_NAMESPACE::__llvm_libc_mb_cur_max, size_t(1));
}

TEST_F(LlvmLibcSetlocale, ANullNameAsks) {
  LIBC_NAMESPACE::setlocale(LC_ALL, "C.UTF-8");
  ASSERT_STREQ(LIBC_NAMESPACE::setlocale(LC_ALL, nullptr), "C.UTF-8");
  ASSERT_STREQ(LIBC_NAMESPACE::setlocale(LC_CTYPE, nullptr), "C.UTF-8");
}

TEST_F(LlvmLibcSetlocale, RejectsACategoryThatIsNotOne) {
  ASSERT_EQ(LIBC_NAMESPACE::setlocale(-1, "C"), static_cast<char *>(nullptr));
  ASSERT_EQ(LIBC_NAMESPACE::setlocale(LC_ALL + 1, "C"),
            static_cast<char *>(nullptr));
}

TEST_F(LlvmLibcSetlocale, ACharacterSetOfMoreThanOneByte) {
  ASSERT_STREQ(LIBC_NAMESPACE::setlocale(LC_ALL, "C.UTF-8"), "C.UTF-8");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(CODESET), "UTF-8");
  ASSERT_EQ(LIBC_NAMESPACE::__llvm_libc_mb_cur_max, size_t(4));
}

TEST_F(LlvmLibcSetlocale, TheCharacterSetIsReadEitherWayItIsWritten) {
  ASSERT_STREQ(LIBC_NAMESPACE::setlocale(LC_ALL, "C.utf8"), "C.utf8");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(CODESET), "UTF-8");
  ASSERT_STREQ(LIBC_NAMESPACE::setlocale(LC_ALL, "en_GB.UTF-8"), "en_GB.UTF-8");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(CODESET), "UTF-8");
  // A modifier may follow the character set and is not part of it.
  ASSERT_STREQ(LIBC_NAMESPACE::setlocale(LC_ALL, "sr_RS.UTF-8@latin"),
               "sr_RS.UTF-8@latin");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(CODESET), "UTF-8");
}

TEST_F(LlvmLibcSetlocale, RefusesACharacterSetItCannotHonour) {
  ASSERT_EQ(LIBC_NAMESPACE::setlocale(LC_ALL, "ja_JP.EUC-JP"),
            static_cast<char *>(nullptr));
  ASSERT_EQ(LIBC_NAMESPACE::setlocale(LC_ALL, "ru_RU.KOI8-R"),
            static_cast<char *>(nullptr));
  // Refusing leaves the locale as it was.
  ASSERT_STREQ(LIBC_NAMESPACE::setlocale(LC_ALL, nullptr), "C");
}

TEST_F(LlvmLibcSetlocale, TheEmptyNameMeansWhatTheEnvironmentSays) {
  LIBC_NAMESPACE::setenv("LANG", "C.UTF-8", 1);
  ASSERT_STREQ(LIBC_NAMESPACE::setlocale(LC_ALL, ""), "C.UTF-8");
  ASSERT_STREQ(LIBC_NAMESPACE::nl_langinfo(CODESET), "UTF-8");
}

TEST_F(LlvmLibcSetlocale, LcAllOverridesTheRest) {
  LIBC_NAMESPACE::setenv("LANG", "C", 1);
  LIBC_NAMESPACE::setenv("LC_CTYPE", "C", 1);
  LIBC_NAMESPACE::setenv("LC_ALL", "C.UTF-8", 1);
  ASSERT_STREQ(LIBC_NAMESPACE::setlocale(LC_ALL, ""), "C.UTF-8");
}

TEST_F(LlvmLibcSetlocale, TheCategoryComesBeforeLang) {
  LIBC_NAMESPACE::setenv("LANG", "C", 1);
  LIBC_NAMESPACE::setenv("LC_CTYPE", "C.UTF-8", 1);
  ASSERT_STREQ(LIBC_NAMESPACE::setlocale(LC_CTYPE, ""), "C.UTF-8");
}

TEST_F(LlvmLibcSetlocale, AnEmptyVariableCountsAsUnset) {
  LIBC_NAMESPACE::setenv("LC_ALL", "", 1);
  LIBC_NAMESPACE::setenv("LANG", "C.UTF-8", 1);
  ASSERT_STREQ(LIBC_NAMESPACE::setlocale(LC_ALL, ""), "C.UTF-8");
}

TEST_F(LlvmLibcSetlocale, TheCLocaleWhenTheEnvironmentSaysNothing) {
  ASSERT_STREQ(LIBC_NAMESPACE::setlocale(LC_ALL, ""), "C");
  ASSERT_EQ(LIBC_NAMESPACE::__llvm_libc_mb_cur_max, size_t(1));
}

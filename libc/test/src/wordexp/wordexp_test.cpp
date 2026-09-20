//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for wordexp and wordfree
///
//===----------------------------------------------------------------------===//

#include "hdr/types/wordexp_t.h"
#include "hdr/wordexp_macros.h"
#include "src/stdlib/setenv.h"
#include "src/stdlib/unsetenv.h"
#include "src/wordexp/wordexp.h"
#include "src/wordexp/wordfree.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcWordexpTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {
struct Words {
  wordexp_t we = {};
  ~Words() { LIBC_NAMESPACE::wordfree(&we); }
};
} // anonymous namespace

TEST_F(LlvmLibcWordexpTest, SplitsOnWhitespace) {
  Words w;
  ASSERT_EQ(LIBC_NAMESPACE::wordexp("one two\tthree", &w.we, 0), 0);
  ASSERT_EQ(w.we.we_wordc, size_t(3));
  EXPECT_STREQ(w.we.we_wordv[0], "one");
  EXPECT_STREQ(w.we.we_wordv[1], "two");
  EXPECT_STREQ(w.we.we_wordv[2], "three");
  EXPECT_TRUE(w.we.we_wordv[3] == nullptr);
}

TEST_F(LlvmLibcWordexpTest, QuotesKeepWordsTogether) {
  Words w;
  ASSERT_EQ(LIBC_NAMESPACE::wordexp("'a b' \"c d\" e\\ f", &w.we, 0), 0);
  ASSERT_EQ(w.we.we_wordc, size_t(3));
  EXPECT_STREQ(w.we.we_wordv[0], "a b");
  EXPECT_STREQ(w.we.we_wordv[1], "c d");
  EXPECT_STREQ(w.we.we_wordv[2], "e f");
}

TEST_F(LlvmLibcWordexpTest, ExpandsParameters) {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("WORDEXP_TEST_VAR", "x y", 1), 0);
  Words w;
  ASSERT_EQ(LIBC_NAMESPACE::wordexp("$WORDEXP_TEST_VAR ${WORDEXP_TEST_VAR}",
                                    &w.we, 0),
            0);
  // Unquoted, the value is split into fields; the result is four words.
  ASSERT_EQ(w.we.we_wordc, size_t(4));
  EXPECT_STREQ(w.we.we_wordv[0], "x");
  EXPECT_STREQ(w.we.we_wordv[1], "y");
  EXPECT_STREQ(w.we.we_wordv[2], "x");
  EXPECT_STREQ(w.we.we_wordv[3], "y");

  Words quoted;
  ASSERT_EQ(LIBC_NAMESPACE::wordexp("\"$WORDEXP_TEST_VAR\"", &quoted.we, 0), 0);
  ASSERT_EQ(quoted.we.we_wordc, size_t(1));
  EXPECT_STREQ(quoted.we.we_wordv[0], "x y");
  LIBC_NAMESPACE::unsetenv("WORDEXP_TEST_VAR");
}

TEST_F(LlvmLibcWordexpTest, AnUnsetVariableIsEmptyUnlessUndefIsAsked) {
  LIBC_NAMESPACE::unsetenv("WORDEXP_TEST_UNSET");
  Words w;
  ASSERT_EQ(LIBC_NAMESPACE::wordexp("a$WORDEXP_TEST_UNSET b", &w.we, 0), 0);
  ASSERT_EQ(w.we.we_wordc, size_t(2));
  EXPECT_STREQ(w.we.we_wordv[0], "a");

  Words strict;
  EXPECT_EQ(LIBC_NAMESPACE::wordexp("$WORDEXP_TEST_UNSET", &strict.we,
                                    WRDE_UNDEF),
            WRDE_BADVAL);
}

TEST_F(LlvmLibcWordexpTest, TildeExpandsToHome) {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("HOME", "/wordexp/home", 1), 0);
  Words w;
  ASSERT_EQ(LIBC_NAMESPACE::wordexp("~ ~/sub", &w.we, 0), 0);
  ASSERT_EQ(w.we.we_wordc, size_t(2));
  EXPECT_STREQ(w.we.we_wordv[0], "/wordexp/home");
  EXPECT_STREQ(w.we.we_wordv[1], "/wordexp/home/sub");
}

// No command is ever run, so the forms that ask for one are refused with or
// without WRDE_NOCMD.
TEST_F(LlvmLibcWordexpTest, CommandSubstitutionIsRefused) {
  Words w;
  EXPECT_EQ(LIBC_NAMESPACE::wordexp("$(echo hi)", &w.we, 0), WRDE_CMDSUB);
  EXPECT_EQ(LIBC_NAMESPACE::wordexp("$(echo hi)", &w.we, WRDE_NOCMD),
            WRDE_CMDSUB);
  EXPECT_EQ(LIBC_NAMESPACE::wordexp("`echo hi`", &w.we, 0), WRDE_CMDSUB);
}

TEST_F(LlvmLibcWordexpTest, UnquotedShellOperatorsAreRefused) {
  Words w;
  EXPECT_EQ(LIBC_NAMESPACE::wordexp("a | b", &w.we, 0), WRDE_BADCHAR);
  EXPECT_EQ(LIBC_NAMESPACE::wordexp("a; b", &w.we, 0), WRDE_BADCHAR);
  EXPECT_EQ(LIBC_NAMESPACE::wordexp("a > b", &w.we, 0), WRDE_BADCHAR);
  // Quoted they are ordinary characters.
  ASSERT_EQ(LIBC_NAMESPACE::wordexp("'a|b'", &w.we, 0), 0);
  ASSERT_EQ(w.we.we_wordc, size_t(1));
  EXPECT_STREQ(w.we.we_wordv[0], "a|b");
}

TEST_F(LlvmLibcWordexpTest, AnUnclosedQuoteIsASyntaxError) {
  Words w;
  EXPECT_EQ(LIBC_NAMESPACE::wordexp("'abc", &w.we, 0), WRDE_SYNTAX);
  EXPECT_EQ(LIBC_NAMESPACE::wordexp("\"abc", &w.we, 0), WRDE_SYNTAX);
}

TEST_F(LlvmLibcWordexpTest, AppendAddsToWhatIsThere) {
  Words w;
  ASSERT_EQ(LIBC_NAMESPACE::wordexp("a b", &w.we, 0), 0);
  ASSERT_EQ(LIBC_NAMESPACE::wordexp("c", &w.we, WRDE_APPEND), 0);
  ASSERT_EQ(w.we.we_wordc, size_t(3));
  EXPECT_STREQ(w.we.we_wordv[0], "a");
  EXPECT_STREQ(w.we.we_wordv[2], "c");
  EXPECT_TRUE(w.we.we_wordv[3] == nullptr);
}

TEST_F(LlvmLibcWordexpTest, DooffsLeavesEmptySlotsAtTheFront) {
  Words w;
  w.we.we_offs = 2;
  ASSERT_EQ(LIBC_NAMESPACE::wordexp("a b", &w.we, WRDE_DOOFFS), 0);
  ASSERT_EQ(w.we.we_wordc, size_t(2));
  EXPECT_TRUE(w.we.we_wordv[0] == nullptr);
  EXPECT_TRUE(w.we.we_wordv[1] == nullptr);
  EXPECT_STREQ(w.we.we_wordv[2], "a");
  EXPECT_STREQ(w.we.we_wordv[3], "b");
  EXPECT_TRUE(w.we.we_wordv[4] == nullptr);
}

TEST_F(LlvmLibcWordexpTest, APatternThatMatchesNothingStaysAsWritten) {
  Words w;
  ASSERT_EQ(LIBC_NAMESPACE::wordexp("/nonexistent-dir/*.nothing", &w.we, 0), 0);
  ASSERT_EQ(w.we.we_wordc, size_t(1));
  EXPECT_STREQ(w.we.we_wordv[0], "/nonexistent-dir/*.nothing");
}

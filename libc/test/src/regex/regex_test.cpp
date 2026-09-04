//===-- Unittests for the POSIX regular expression functions --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/regex_macros.h"
#include "hdr/types/regex_t.h"
#include "hdr/types/regmatch_t.h"
#include "src/regex/regcomp.h"
#include "src/regex/regexec.h"
#include "src/regex/regfree.h"
#include "test/UnitTest/Test.h"

namespace {

// Compiles `pattern` and reports whether it matches `text`.
bool matches(const char *pattern, const char *text, int cflags = REG_EXTENDED,
             int eflags = 0) {
  regex_t re;
  if (LIBC_NAMESPACE::regcomp(&re, pattern, cflags) != 0)
    return false;
  int rc = LIBC_NAMESPACE::regexec(&re, text, 0, nullptr, eflags);
  LIBC_NAMESPACE::regfree(&re);
  return rc == 0;
}

int compile_error(const char *pattern, int cflags = REG_EXTENDED) {
  regex_t re;
  int rc = LIBC_NAMESPACE::regcomp(&re, pattern, cflags);
  if (rc == 0)
    LIBC_NAMESPACE::regfree(&re);
  return rc;
}

} // anonymous namespace

TEST(LlvmLibcRegexTest, Literals) {
  EXPECT_TRUE(matches("abc", "abc"));
  EXPECT_TRUE(matches("abc", "xxabcxx"));
  EXPECT_FALSE(matches("abc", "abd"));
  EXPECT_TRUE(matches("", "anything"));
}

TEST(LlvmLibcRegexTest, Anchors) {
  // This is the case the kernel's relocs tool depends on.
  EXPECT_TRUE(matches("^real_mode_seg$", "real_mode_seg"));
  EXPECT_FALSE(matches("^real_mode_seg$", "xreal_mode_seg"));
  EXPECT_FALSE(matches("^real_mode_seg$", "real_mode_segx"));
  EXPECT_TRUE(matches("^abc", "abcdef"));
  EXPECT_FALSE(matches("^abc", "xabcdef"));
  EXPECT_TRUE(matches("def$", "abcdef"));
  EXPECT_FALSE(matches("def$", "abcdefx"));
}

TEST(LlvmLibcRegexTest, AnyCharacter) {
  EXPECT_TRUE(matches("a.c", "abc"));
  EXPECT_TRUE(matches("a.c", "a c"));
  EXPECT_FALSE(matches("a.c", "ac"));
}

TEST(LlvmLibcRegexTest, Repetition) {
  EXPECT_TRUE(matches("ab*c", "ac"));
  EXPECT_TRUE(matches("ab*c", "abbbc"));
  EXPECT_FALSE(matches("ab+c", "ac"));
  EXPECT_TRUE(matches("ab+c", "abc"));
  EXPECT_TRUE(matches("ab?c", "ac"));
  EXPECT_TRUE(matches("ab?c", "abc"));
  EXPECT_FALSE(matches("ab?c", "abbc"));
}

TEST(LlvmLibcRegexTest, Bounds) {
  EXPECT_FALSE(matches("^a{2,3}$", "a"));
  EXPECT_TRUE(matches("^a{2,3}$", "aa"));
  EXPECT_TRUE(matches("^a{2,3}$", "aaa"));
  EXPECT_FALSE(matches("^a{2,3}$", "aaaa"));
  EXPECT_TRUE(matches("^a{2}$", "aa"));
  EXPECT_FALSE(matches("^a{2}$", "aaa"));
  EXPECT_TRUE(matches("^a{2,}$", "aaaaa"));
}

TEST(LlvmLibcRegexTest, Alternation) {
  EXPECT_TRUE(matches("^(cat|dog)$", "cat"));
  EXPECT_TRUE(matches("^(cat|dog)$", "dog"));
  EXPECT_FALSE(matches("^(cat|dog)$", "cow"));
  // The pattern shape the kernel's relocs tool actually uses.
  EXPECT_TRUE(matches("^(xen_irq_disable_direct_reloc$|"
                      "xen_save_fl_direct_reloc$)",
                      "xen_save_fl_direct_reloc"));
}

TEST(LlvmLibcRegexTest, BracketExpressions) {
  EXPECT_TRUE(matches("^[abc]$", "b"));
  EXPECT_FALSE(matches("^[abc]$", "d"));
  EXPECT_TRUE(matches("^[a-z]+$", "hello"));
  EXPECT_FALSE(matches("^[a-z]+$", "Hello"));
  EXPECT_TRUE(matches("^[^0-9]+$", "abc"));
  EXPECT_FALSE(matches("^[^0-9]+$", "ab3"));
  // A ']' first is a literal, and a '-' last is a literal.
  EXPECT_TRUE(matches("^[]]$", "]"));
  EXPECT_TRUE(matches("^[a-]$", "-"));
}

TEST(LlvmLibcRegexTest, NamedClasses) {
  EXPECT_TRUE(matches("^[[:digit:]]+$", "12345"));
  EXPECT_FALSE(matches("^[[:digit:]]+$", "12a45"));
  EXPECT_TRUE(matches("^[[:alpha:]]+$", "abcXYZ"));
  EXPECT_TRUE(matches("^[[:alnum:]_]+$", "a_1"));
  EXPECT_TRUE(matches("^[[:space:]]$", " "));
  EXPECT_TRUE(matches("^[[:xdigit:]]+$", "deadBEEF01"));
  EXPECT_FALSE(matches("^[[:xdigit:]]+$", "ghij"));
}

TEST(LlvmLibcRegexTest, CaseInsensitive) {
  EXPECT_TRUE(matches("^abc$", "ABC", REG_EXTENDED | REG_ICASE));
  EXPECT_FALSE(matches("^abc$", "ABC", REG_EXTENDED));
  EXPECT_TRUE(matches("^[a-z]+$", "AbC", REG_EXTENDED | REG_ICASE));
}

TEST(LlvmLibcRegexTest, BasicDialect) {
  // In a basic expression these are literals unless backslash escaped.
  EXPECT_TRUE(matches("a+b", "a+b", 0));
  EXPECT_TRUE(matches("a\\+", "aaa", 0));
  EXPECT_TRUE(matches("^(a)$", "(a)", 0));
  EXPECT_TRUE(matches("^\\(a\\)$", "a", 0));
  // But '*' is an operator in both.
  EXPECT_TRUE(matches("^ab*$", "abbb", 0));
}

TEST(LlvmLibcRegexTest, Subexpressions) {
  regex_t re;
  ASSERT_EQ(LIBC_NAMESPACE::regcomp(&re, "^([a-z]+)-([0-9]+)$", REG_EXTENDED),
            0);
  EXPECT_EQ(re.re_nsub, size_t(2));
  regmatch_t m[3];
  ASSERT_EQ(LIBC_NAMESPACE::regexec(&re, "abc-123", 3, m, 0), 0);
  EXPECT_EQ(m[0].rm_so, regoff_t(0));
  EXPECT_EQ(m[0].rm_eo, regoff_t(7));
  EXPECT_EQ(m[1].rm_so, regoff_t(0));
  EXPECT_EQ(m[1].rm_eo, regoff_t(3));
  EXPECT_EQ(m[2].rm_so, regoff_t(4));
  EXPECT_EQ(m[2].rm_eo, regoff_t(7));
  LIBC_NAMESPACE::regfree(&re);
}

TEST(LlvmLibcRegexTest, MatchOffsets) {
  regex_t re;
  ASSERT_EQ(LIBC_NAMESPACE::regcomp(&re, "b+", REG_EXTENDED), 0);
  regmatch_t m[1];
  ASSERT_EQ(LIBC_NAMESPACE::regexec(&re, "aabbbcc", 1, m, 0), 0);
  // Leftmost, and greedy so the whole run is taken.
  EXPECT_EQ(m[0].rm_so, regoff_t(2));
  EXPECT_EQ(m[0].rm_eo, regoff_t(5));
  LIBC_NAMESPACE::regfree(&re);
}

TEST(LlvmLibcRegexTest, NotbolAndNoteol) {
  EXPECT_FALSE(matches("^abc", "abc", REG_EXTENDED, REG_NOTBOL));
  EXPECT_FALSE(matches("abc$", "abc", REG_EXTENDED, REG_NOTEOL));
  EXPECT_TRUE(matches("abc", "abc", REG_EXTENDED, REG_NOTBOL));
}

TEST(LlvmLibcRegexTest, Backreferences) {
  EXPECT_TRUE(matches("^(ab)\\1$", "abab"));
  EXPECT_FALSE(matches("^(ab)\\1$", "abcd"));
}

TEST(LlvmLibcRegexTest, CompileErrors) {
  EXPECT_EQ(compile_error("a["), REG_EBRACK);
  EXPECT_EQ(compile_error("(a"), REG_EPAREN);
  EXPECT_EQ(compile_error("a)"), REG_EPAREN);
  EXPECT_EQ(compile_error("a\\"), REG_EESCAPE);
  EXPECT_EQ(compile_error("a{2,1}"), REG_BADBR);
  EXPECT_EQ(compile_error("[z-a]"), REG_ERANGE);
  EXPECT_EQ(compile_error("*a"), REG_BADRPT);
}

TEST(LlvmLibcRegexTest, NoSubSuppressesOffsets) {
  regex_t re;
  ASSERT_EQ(LIBC_NAMESPACE::regcomp(&re, "(a)(b)", REG_EXTENDED | REG_NOSUB),
            0);
  regmatch_t m[3];
  for (int i = 0; i < 3; ++i)
    m[i].rm_so = m[i].rm_eo = -99;
  ASSERT_EQ(LIBC_NAMESPACE::regexec(&re, "ab", 3, m, 0), 0);
  EXPECT_EQ(m[0].rm_so, regoff_t(-99));
  LIBC_NAMESPACE::regfree(&re);
}

TEST(LlvmLibcRegexTest, NoMatchReturnsRegNomatch) {
  regex_t re;
  ASSERT_EQ(LIBC_NAMESPACE::regcomp(&re, "^zzz$", REG_EXTENDED), 0);
  EXPECT_EQ(LIBC_NAMESPACE::regexec(&re, "abc", 0, nullptr, 0), REG_NOMATCH);
  LIBC_NAMESPACE::regfree(&re);
}

TEST(LlvmLibcRegexTest, PathologicalPatternTerminates) {
  // A nested empty repetition must not loop forever.
  EXPECT_FALSE(matches("^(a*)*$", "aaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
}

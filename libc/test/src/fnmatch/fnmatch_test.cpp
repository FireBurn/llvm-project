//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for fnmatch.
///
//===----------------------------------------------------------------------===//

#include "hdr/fnmatch_macros.h"
#include "src/fnmatch/fnmatch.h"
#include "test/UnitTest/Test.h"

#define EXPECT_MATCH(pattern, string, flags)                                   \
  EXPECT_EQ(LIBC_NAMESPACE::fnmatch(pattern, string, flags), 0)
#define EXPECT_NO_MATCH(pattern, string, flags)                                \
  EXPECT_EQ(LIBC_NAMESPACE::fnmatch(pattern, string, flags), FNM_NOMATCH)

TEST(LlvmLibcFnmatchTest, Literal) {
  EXPECT_MATCH("test", "test", 0);
  EXPECT_MATCH("", "", 0);
  EXPECT_NO_MATCH("test", "Test", 0);
  EXPECT_NO_MATCH("test", "tes", 0);
  EXPECT_NO_MATCH("tes", "test", 0);
  EXPECT_NO_MATCH("", "a", 0);
  EXPECT_NO_MATCH("a", "", 0);
}

TEST(LlvmLibcFnmatchTest, Star) {
  EXPECT_MATCH("*", "anything", 0);
  EXPECT_MATCH("*", "", 0);
  EXPECT_MATCH("*.txt", "a.txt", 0);
  EXPECT_MATCH("*.txt", "ab.txt", 0);
  EXPECT_MATCH("*.txt", ".txt", 0);
  EXPECT_MATCH("a*", "a.txt", 0);
  EXPECT_MATCH("a*t", "a.txt", 0);
  EXPECT_MATCH("*.*", "a.txt", 0);
  EXPECT_MATCH("**", "abc", 0);
  EXPECT_MATCH("a*b*c", "axxbyyc", 0);
  EXPECT_NO_MATCH("*.txt", "a.log", 0);
  EXPECT_NO_MATCH("a*b*c", "axxbyy", 0);
}

TEST(LlvmLibcFnmatchTest, StarBacktracks) {
  // The greedy star has to give characters back for the tail to match.
  EXPECT_MATCH("*abc", "xxabcxxabc", 0);
  EXPECT_MATCH("*a*b", "aaab", 0);
  EXPECT_MATCH("a*a*a", "aaaa", 0);
  EXPECT_NO_MATCH("*abcd", "xxabcxxabc", 0);
}

TEST(LlvmLibcFnmatchTest, QuestionMark) {
  EXPECT_MATCH("?", "a", 0);
  EXPECT_MATCH("?.txt", "a.txt", 0);
  EXPECT_MATCH("a?c", "abc", 0);
  EXPECT_NO_MATCH("?", "", 0);
  EXPECT_NO_MATCH("?", "ab", 0);
  EXPECT_NO_MATCH("?.txt", "ab.txt", 0);
}

TEST(LlvmLibcFnmatchTest, Brackets) {
  EXPECT_MATCH("[abc]", "b", 0);
  EXPECT_MATCH("[a-z]", "q", 0);
  EXPECT_MATCH("[a-z0-9]", "5", 0);
  EXPECT_MATCH("[!abc]", "d", 0);
  EXPECT_MATCH("[^abc]", "d", 0);
  EXPECT_MATCH("[ab].txt", "a.txt", 0);
  EXPECT_NO_MATCH("[abc]", "d", 0);
  EXPECT_NO_MATCH("[a-z]", "Q", 0);
  EXPECT_NO_MATCH("[!abc]", "a", 0);

  // A ']' first in the set is a literal, and a '-' last in the set is too.
  EXPECT_MATCH("[]]", "]", 0);
  EXPECT_MATCH("[!]]", "a", 0);
  EXPECT_MATCH("[a-]", "-", 0);
  EXPECT_MATCH("[a-]", "a", 0);

  // An unterminated set is a literal '['.
  EXPECT_MATCH("[abc", "[abc", 0);
  EXPECT_NO_MATCH("[abc", "a", 0);
}

TEST(LlvmLibcFnmatchTest, CharacterClasses) {
  EXPECT_MATCH("[[:alpha:]]", "x", 0);
  EXPECT_MATCH("[[:digit:]]", "7", 0);
  EXPECT_MATCH("[[:alnum:]]", "7", 0);
  EXPECT_MATCH("[[:upper:]]", "X", 0);
  EXPECT_MATCH("[[:lower:]]", "x", 0);
  EXPECT_MATCH("[[:space:]]", " ", 0);
  EXPECT_MATCH("[[:xdigit:]]", "f", 0);
  EXPECT_MATCH("[[:punct:]]", ",", 0);
  EXPECT_MATCH("[[:digit:]abc]", "b", 0);
  EXPECT_NO_MATCH("[[:alpha:]]", "7", 0);
  EXPECT_NO_MATCH("[[:digit:]]", "x", 0);
  EXPECT_NO_MATCH("[[:upper:]]", "x", 0);
}

TEST(LlvmLibcFnmatchTest, Escapes) {
  EXPECT_MATCH("\\*", "*", 0);
  EXPECT_MATCH("\\?", "?", 0);
  EXPECT_MATCH("a\\*b", "a*b", 0);
  EXPECT_NO_MATCH("\\*", "a", 0);

  // With FNM_NOESCAPE a backslash is just a backslash.
  EXPECT_MATCH("\\*", "\\a", FNM_NOESCAPE);
  EXPECT_NO_MATCH("\\*", "*", FNM_NOESCAPE);
}

TEST(LlvmLibcFnmatchTest, Pathname) {
  // A '*' does not cross a '/' when slashes are significant.
  EXPECT_MATCH("*", "abc", FNM_PATHNAME);
  EXPECT_MATCH("*/*", "a/b", FNM_PATHNAME);
  EXPECT_MATCH("a/*", "a/b", FNM_PATHNAME);
  EXPECT_NO_MATCH("*", "a/b", FNM_PATHNAME);
  EXPECT_NO_MATCH("a*c", "a/c", FNM_PATHNAME);
  EXPECT_NO_MATCH("?", "/", FNM_PATHNAME);
  EXPECT_NO_MATCH("[/]", "/", FNM_PATHNAME);

  // Without it, a '*' happily matches a slash.
  EXPECT_MATCH("*", "a/b", 0);
  EXPECT_MATCH("a*c", "a/c", 0);
}

TEST(LlvmLibcFnmatchTest, Period) {
  // A leading period has to be matched explicitly.
  EXPECT_NO_MATCH("*", ".hidden", FNM_PERIOD);
  EXPECT_NO_MATCH("?hidden", ".hidden", FNM_PERIOD);
  EXPECT_NO_MATCH("[.]hidden", ".hidden", FNM_PERIOD);
  EXPECT_MATCH(".*", ".hidden", FNM_PERIOD);
  EXPECT_MATCH("*", "visible", FNM_PERIOD);
  EXPECT_MATCH("a.*", "a.b", FNM_PERIOD);

  // Only leading, and only after a slash when slashes are significant.
  EXPECT_NO_MATCH("a/*", "a/.b", FNM_PERIOD | FNM_PATHNAME);
  EXPECT_MATCH("a/.*", "a/.b", FNM_PERIOD | FNM_PATHNAME);
  EXPECT_MATCH("a*", "a/.b", FNM_PERIOD);

  // Without FNM_PERIOD a period is an ordinary character.
  EXPECT_MATCH("*", ".hidden", 0);
}

TEST(LlvmLibcFnmatchTest, CaseFold) {
  EXPECT_MATCH("test", "TEST", FNM_CASEFOLD);
  EXPECT_MATCH("*.TXT", "a.txt", FNM_CASEFOLD);
  EXPECT_MATCH("[a-z]", "Q", FNM_CASEFOLD);
  EXPECT_MATCH("[A-Z]", "q", FNM_CASEFOLD);
  EXPECT_NO_MATCH("test", "TEST", 0);
}

TEST(LlvmLibcFnmatchTest, LeadingDir) {
  // The pattern may match a leading directory of the string.
  EXPECT_MATCH("a", "a/b", FNM_LEADING_DIR);
  EXPECT_MATCH("*", "a/b", FNM_LEADING_DIR | FNM_PATHNAME);
  EXPECT_MATCH("a/b", "a/b", FNM_LEADING_DIR);
  EXPECT_NO_MATCH("a", "ab", FNM_LEADING_DIR);
  EXPECT_NO_MATCH("b", "a/b", FNM_LEADING_DIR);

  // The pattern can run out part way through the string, not only where the
  // string itself ends.
  EXPECT_MATCH("*a", "a/b", FNM_LEADING_DIR);
  EXPECT_MATCH("*a", "a/b/c", FNM_LEADING_DIR);
  EXPECT_MATCH("*a", "a/", FNM_LEADING_DIR);
  EXPECT_MATCH("*a", "a/.b", FNM_LEADING_DIR | FNM_PERIOD);
  EXPECT_MATCH("*a", "a/b", FNM_LEADING_DIR | FNM_PATHNAME);
  EXPECT_MATCH("*/", "a//b", FNM_LEADING_DIR);
  EXPECT_MATCH("a**b", "a/b/c", FNM_LEADING_DIR);
}

TEST(LlvmLibcFnmatchTest, PathologicalPatternsTerminate) {
  // A run of stars must not send the matcher exponential.
  EXPECT_NO_MATCH("*a*a*a*a*a*a*a*a*b", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 0);
  EXPECT_MATCH("*a*a*a*a*a*a*a*a*b", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab", 0);
}

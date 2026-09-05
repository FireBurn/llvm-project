//===-- Unittests for glob and globfree -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/fcntl_macros.h"
#include "hdr/glob_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/glob_t.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/libc_errno.h"
#include "src/fcntl/open.h"
#include "src/glob/glob.h"
#include "src/glob/globfree.h"
#include "src/stdio/remove.h"
#include "src/sys/stat/mkdir.h"
#include "src/unistd/close.h"
#include "src/unistd/rmdir.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcGlobTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

namespace {

constexpr const char *ROOT = "glob.testdir";

void make_file(const char *path) {
  int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);
}

// glob.testdir/{a.txt, b.txt, c.log, .hidden, sub/{d.txt, e.log}}
void tear_down_tree();

void build_tree() {
  // A previous run may have left the tree behind, and mkdir would then fail
  // with EEXIST and leave that in errno.
  tear_down_tree();
  ASSERT_EQ(LIBC_NAMESPACE::mkdir(ROOT, S_IRWXU), 0);
  ASSERT_EQ(LIBC_NAMESPACE::mkdir("glob.testdir/sub", S_IRWXU), 0);
  make_file("glob.testdir/a.txt");
  make_file("glob.testdir/b.txt");
  make_file("glob.testdir/c.log");
  make_file("glob.testdir/.hidden");
  make_file("glob.testdir/sub/d.txt");
  make_file("glob.testdir/sub/e.log");
}

void tear_down_tree() {
  LIBC_NAMESPACE::remove("glob.testdir/sub/d.txt");
  LIBC_NAMESPACE::remove("glob.testdir/sub/e.log");
  LIBC_NAMESPACE::rmdir("glob.testdir/sub");
  LIBC_NAMESPACE::remove("glob.testdir/a.txt");
  LIBC_NAMESPACE::remove("glob.testdir/b.txt");
  LIBC_NAMESPACE::remove("glob.testdir/c.log");
  LIBC_NAMESPACE::remove("glob.testdir/.hidden");
  LIBC_NAMESPACE::rmdir(ROOT);
  // Removing what is not there is expected, so the errno it leaves is not
  // the test's business.
  libc_errno = 0;
}

} // anonymous namespace

TEST_F(LlvmLibcGlobTest, MatchesAndSorts) {
  build_tree();

  glob_t g;
  ASSERT_EQ(LIBC_NAMESPACE::glob("glob.testdir/*.txt", 0, nullptr, &g), 0);
  ASSERT_EQ(g.gl_pathc, size_t(2));
  // The results come back in order without the caller asking.
  EXPECT_TRUE(string_view(g.gl_pathv[0]) == "glob.testdir/a.txt");
  EXPECT_TRUE(string_view(g.gl_pathv[1]) == "glob.testdir/b.txt");
  EXPECT_TRUE(g.gl_pathv[2] == nullptr);
  LIBC_NAMESPACE::globfree(&g);

  tear_down_tree();
}

TEST_F(LlvmLibcGlobTest, LeadingPeriodIsNotMatchedByAStar) {
  build_tree();

  glob_t g;
  ASSERT_EQ(LIBC_NAMESPACE::glob("glob.testdir/*", 0, nullptr, &g), 0);
  // A star does not match the leading period of .hidden, so the four visible
  // names are all that come back.
  ASSERT_EQ(g.gl_pathc, size_t(4));
  for (size_t i = 0; i < g.gl_pathc; ++i)
    EXPECT_FALSE(string_view(g.gl_pathv[i]) == "glob.testdir/.hidden");
  LIBC_NAMESPACE::globfree(&g);

  tear_down_tree();
}

TEST_F(LlvmLibcGlobTest, MatchesAcrossADirectoryComponent) {
  build_tree();

  glob_t g;
  ASSERT_EQ(LIBC_NAMESPACE::glob("glob.testdir/*/*.log", 0, nullptr, &g), 0);
  ASSERT_EQ(g.gl_pathc, size_t(1));
  EXPECT_TRUE(string_view(g.gl_pathv[0]) == "glob.testdir/sub/e.log");
  LIBC_NAMESPACE::globfree(&g);

  tear_down_tree();
}

TEST_F(LlvmLibcGlobTest, QuestionMarkAndBrackets) {
  build_tree();

  glob_t g;
  ASSERT_EQ(LIBC_NAMESPACE::glob("glob.testdir/?.txt", 0, nullptr, &g), 0);
  EXPECT_EQ(g.gl_pathc, size_t(2));
  LIBC_NAMESPACE::globfree(&g);

  ASSERT_EQ(LIBC_NAMESPACE::glob("glob.testdir/[ab].txt", 0, nullptr, &g), 0);
  EXPECT_EQ(g.gl_pathc, size_t(2));
  LIBC_NAMESPACE::globfree(&g);

  ASSERT_EQ(LIBC_NAMESPACE::glob("glob.testdir/[a].txt", 0, nullptr, &g), 0);
  ASSERT_EQ(g.gl_pathc, size_t(1));
  EXPECT_TRUE(string_view(g.gl_pathv[0]) == "glob.testdir/a.txt");
  LIBC_NAMESPACE::globfree(&g);

  tear_down_tree();
}

TEST_F(LlvmLibcGlobTest, LiteralPatternNeedsThePathToExist) {
  build_tree();

  glob_t g;
  ASSERT_EQ(LIBC_NAMESPACE::glob("glob.testdir/a.txt", 0, nullptr, &g), 0);
  ASSERT_EQ(g.gl_pathc, size_t(1));
  LIBC_NAMESPACE::globfree(&g);

  EXPECT_EQ(LIBC_NAMESPACE::glob("glob.testdir/none.txt", 0, nullptr, &g),
            GLOB_NOMATCH);
  LIBC_NAMESPACE::globfree(&g);
  // A glob which found nothing leaves behind the errno of the lookup that
  // failed, which is what glibc does too.
  libc_errno = 0;

  tear_down_tree();
}

TEST_F(LlvmLibcGlobTest, NoMatch) {
  build_tree();

  glob_t g;
  EXPECT_EQ(LIBC_NAMESPACE::glob("glob.testdir/*.zzz", 0, nullptr, &g),
            GLOB_NOMATCH);
  LIBC_NAMESPACE::globfree(&g);
  libc_errno = 0;

  // GLOB_NOCHECK makes an unmatched pattern stand for itself.
  ASSERT_EQ(
      LIBC_NAMESPACE::glob("glob.testdir/*.zzz", GLOB_NOCHECK, nullptr, &g), 0);
  ASSERT_EQ(g.gl_pathc, size_t(1));
  EXPECT_TRUE(string_view(g.gl_pathv[0]) == "glob.testdir/*.zzz");
  LIBC_NAMESPACE::globfree(&g);

  tear_down_tree();
}

TEST_F(LlvmLibcGlobTest, Mark) {
  build_tree();

  glob_t g;
  ASSERT_EQ(LIBC_NAMESPACE::glob("glob.testdir/s*", GLOB_MARK, nullptr, &g), 0);
  ASSERT_EQ(g.gl_pathc, size_t(1));
  // GLOB_MARK puts a slash on the names which are directories.
  EXPECT_TRUE(string_view(g.gl_pathv[0]) == "glob.testdir/sub/");
  LIBC_NAMESPACE::globfree(&g);

  tear_down_tree();
}

TEST_F(LlvmLibcGlobTest, TrailingSlashInThePattern) {
  build_tree();

  glob_t g;
  ASSERT_EQ(LIBC_NAMESPACE::glob("glob.testdir/sub/", 0, nullptr, &g), 0);
  ASSERT_EQ(g.gl_pathc, size_t(1));
  EXPECT_TRUE(string_view(g.gl_pathv[0]) == "glob.testdir/sub/");
  LIBC_NAMESPACE::globfree(&g);

  // The slash GLOB_MARK asks for is already there, so it is not doubled.
  ASSERT_EQ(LIBC_NAMESPACE::glob("glob.testdir/sub/", GLOB_MARK, nullptr, &g),
            0);
  ASSERT_EQ(g.gl_pathc, size_t(1));
  EXPECT_TRUE(string_view(g.gl_pathv[0]) == "glob.testdir/sub/");
  LIBC_NAMESPACE::globfree(&g);

  // A trailing slash also means only directories match.
  EXPECT_EQ(LIBC_NAMESPACE::glob("glob.testdir/a.txt/", 0, nullptr, &g),
            GLOB_NOMATCH);
  LIBC_NAMESPACE::globfree(&g);
  libc_errno = 0;

  tear_down_tree();
}

TEST_F(LlvmLibcGlobTest, RepeatedSlashesAreKeptAsWritten) {
  build_tree();

  glob_t g;
  ASSERT_EQ(LIBC_NAMESPACE::glob("glob.testdir//sub", 0, nullptr, &g), 0);
  ASSERT_EQ(g.gl_pathc, size_t(1));
  EXPECT_TRUE(string_view(g.gl_pathv[0]) == "glob.testdir//sub");
  LIBC_NAMESPACE::globfree(&g);

  tear_down_tree();
}

TEST_F(LlvmLibcGlobTest, Append) {
  build_tree();

  glob_t g;
  ASSERT_EQ(LIBC_NAMESPACE::glob("glob.testdir/*.txt", 0, nullptr, &g), 0);
  ASSERT_EQ(g.gl_pathc, size_t(2));
  // A second call with GLOB_APPEND adds to the list rather than replacing it.
  ASSERT_EQ(
      LIBC_NAMESPACE::glob("glob.testdir/*.log", GLOB_APPEND, nullptr, &g), 0);
  ASSERT_EQ(g.gl_pathc, size_t(3));
  EXPECT_TRUE(string_view(g.gl_pathv[2]) == "glob.testdir/c.log");
  EXPECT_TRUE(g.gl_pathv[3] == nullptr);
  LIBC_NAMESPACE::globfree(&g);

  tear_down_tree();
}

TEST_F(LlvmLibcGlobTest, DoOffs) {
  build_tree();

  glob_t g;
  g.gl_offs = 2;
  ASSERT_EQ(
      LIBC_NAMESPACE::glob("glob.testdir/*.txt", GLOB_DOOFFS, nullptr, &g), 0);
  ASSERT_EQ(g.gl_pathc, size_t(2));
  // The reserved slots are left null for the caller to fill in.
  EXPECT_TRUE(g.gl_pathv[0] == nullptr);
  EXPECT_TRUE(g.gl_pathv[1] == nullptr);
  EXPECT_TRUE(string_view(g.gl_pathv[2]) == "glob.testdir/a.txt");
  EXPECT_TRUE(g.gl_pathv[4] == nullptr);
  LIBC_NAMESPACE::globfree(&g);

  tear_down_tree();
}

TEST_F(LlvmLibcGlobTest, NoSort) {
  build_tree();

  glob_t g;
  ASSERT_EQ(
      LIBC_NAMESPACE::glob("glob.testdir/*.txt", GLOB_NOSORT, nullptr, &g), 0);
  // The order is unspecified, but everything still has to be there.
  EXPECT_EQ(g.gl_pathc, size_t(2));
  LIBC_NAMESPACE::globfree(&g);

  tear_down_tree();
}

TEST_F(LlvmLibcGlobTest, UnreadableDirectoryIsNotAnErrorByDefault) {
  glob_t g;
  EXPECT_EQ(LIBC_NAMESPACE::glob("glob.no.such.dir/*", 0, nullptr, &g),
            GLOB_NOMATCH);
  LIBC_NAMESPACE::globfree(&g);

  // With GLOB_ERR the same walk stops instead, because a directory which had
  // to be read could not be.
  EXPECT_EQ(LIBC_NAMESPACE::glob("glob.no.such.dir/*", GLOB_ERR, nullptr, &g),
            GLOB_ABORTED);
  LIBC_NAMESPACE::globfree(&g);
  libc_errno = 0;
}

TEST_F(LlvmLibcGlobTest, GlobErrOnlyAppliesToDirectoriesThatAreRead) {
  build_tree();

  // The last component is literal, so no directory is read and a missing
  // path is simply not a match even under GLOB_ERR.
  glob_t g;
  EXPECT_EQ(LIBC_NAMESPACE::glob("glob.testdir/nodir/x", GLOB_ERR, nullptr, &g),
            GLOB_NOMATCH);
  LIBC_NAMESPACE::globfree(&g);
  libc_errno = 0;

  // A wildcard which turns up files rather than directories skips them
  // rather than treating them as unreadable.
  EXPECT_EQ(LIBC_NAMESPACE::glob("glob.testdir/*/x", GLOB_ERR, nullptr, &g),
            GLOB_NOMATCH);
  LIBC_NAMESPACE::globfree(&g);
  libc_errno = 0;

  tear_down_tree();
}

TEST_F(LlvmLibcGlobTest, GlobfreeToleratesAnEmptyResult) {
  glob_t g;
  g.gl_pathc = 0;
  g.gl_pathv = nullptr;
  g.gl_offs = 0;
  LIBC_NAMESPACE::globfree(&g);
  LIBC_NAMESPACE::globfree(nullptr);
}

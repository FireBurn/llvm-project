//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for fts_open, fts_read, fts_children, fts_set and fts_close.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/fts_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/__support/libc_errno.h"
#include "hdr/types/FTS.h"
#include "hdr/types/FTSENT.h"
#include "src/fcntl/open.h"
#include "src/fts/fts_children.h"
#include "src/fts/fts_close.h"
#include "src/fts/fts_open.h"
#include "src/fts/fts_read.h"
#include "src/fts/fts_set.h"
#include "src/string/strcmp.h"
#include "src/sys/stat/mkdir.h"
#include "src/unistd/close.h"
#include "src/unistd/rmdir.h"
#include "src/unistd/symlink.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcFtsTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {

int by_name(const FTSENT **a, const FTSENT **b) {
  return LIBC_NAMESPACE::strcmp((*a)->fts_name, (*b)->fts_name);
}

// The tree every test walks:
//   root/
//     inner/
//       deep.txt
//     top.txt
//     link -> top.txt
struct Tree {
  // The path is copied as the name libc_make_test_file_path gives may not
  // outlive the expression it is in.
  char root[4096];
  char link[4096];
  char top[4096];
  char deep[4096];
  char inner[4096];

  static void join(const char *a, const char *b, char *out) {
    size_t i = 0;
    for (; a[i] != '\0'; ++i)
      out[i] = a[i];
    size_t j = 0;
    for (; b[j] != '\0'; ++j)
      out[i + j] = b[j];
    out[i + j] = '\0';
  }

  explicit Tree(const char *r) {
    join(r, "", root);
    join(root, "/link", link);
    join(root, "/top.txt", top);
    join(root, "/inner", inner);
    join(root, "/inner/deep.txt", deep);
    LIBC_NAMESPACE::mkdir(root, S_IRWXU);
    LIBC_NAMESPACE::mkdir(inner, S_IRWXU);
    make_file(top);
    make_file(deep);
    LIBC_NAMESPACE::symlink("top.txt", link);
  }

  ~Tree() {
    LIBC_NAMESPACE::unlink(link);
    LIBC_NAMESPACE::unlink(deep);
    LIBC_NAMESPACE::unlink(top);
    LIBC_NAMESPACE::rmdir(inner);
    LIBC_NAMESPACE::rmdir(root);
  }

  static void make_file(const char *path) {
    int fd = LIBC_NAMESPACE::open(path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd >= 0)
      LIBC_NAMESPACE::close(fd);
  }
};

struct Seen {
  int info[16];
  char name[16][32];
  int count = 0;

  void add(const FTSENT *entry) {
    if (count == 16)
      return;
    info[count] = entry->fts_info;
    size_t i = 0;
    for (; entry->fts_name[i] != '\0' && i < 31; ++i)
      name[count][i] = entry->fts_name[i];
    name[count][i] = '\0';
    ++count;
  }

  bool is(int index, int expected_info, const char *expected_name) const {
    return index < count && info[index] == expected_info &&
           LIBC_NAMESPACE::strcmp(name[index], expected_name) == 0;
  }
};

} // anonymous namespace

TEST_F(LlvmLibcFtsTest, WalksInPreAndPostOrder) {
  Tree tree(libc_make_test_file_path("fts_order"));
  char *paths[] = {tree.root, nullptr};
  FTS *fts = LIBC_NAMESPACE::fts_open(paths, FTS_PHYSICAL, by_name);
  ASSERT_TRUE(fts != nullptr);
  Seen seen;
  while (FTSENT *entry = LIBC_NAMESPACE::fts_read(fts))
    seen.add(entry);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_EQ(seen.count, 7);
  EXPECT_EQ(seen.info[0], int(FTS_D));
  EXPECT_TRUE(seen.is(1, FTS_D, "inner"));
  EXPECT_TRUE(seen.is(2, FTS_F, "deep.txt"));
  EXPECT_TRUE(seen.is(3, FTS_DP, "inner"));
  EXPECT_TRUE(seen.is(4, FTS_SL, "link"));
  EXPECT_TRUE(seen.is(5, FTS_F, "top.txt"));
  EXPECT_EQ(seen.info[6], FTS_DP);
  EXPECT_EQ(LIBC_NAMESPACE::fts_close(fts), 0);
}

TEST_F(LlvmLibcFtsTest, EntriesKnowTheirPlace) {
  Tree tree(libc_make_test_file_path("fts_place"));
  char *paths[] = {tree.root, nullptr};
  FTS *fts = LIBC_NAMESPACE::fts_open(paths, FTS_PHYSICAL, by_name);
  ASSERT_TRUE(fts != nullptr);
  FTSENT *root = LIBC_NAMESPACE::fts_read(fts);
  ASSERT_TRUE(root != nullptr);
  EXPECT_EQ(int(root->fts_level), 0);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(root->fts_path, tree.root), 0);
  FTSENT *inner = LIBC_NAMESPACE::fts_read(fts);
  ASSERT_TRUE(inner != nullptr);
  EXPECT_EQ(int(inner->fts_level), 1);
  EXPECT_EQ(inner->fts_parent, root);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(inner->fts_path, tree.inner), 0);
  EXPECT_TRUE(S_ISDIR(inner->fts_statp->st_mode));
  FTSENT *deep = LIBC_NAMESPACE::fts_read(fts);
  ASSERT_TRUE(deep != nullptr);
  EXPECT_EQ(int(deep->fts_level), 2);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(deep->fts_accpath, tree.deep), 0);
  EXPECT_EQ(LIBC_NAMESPACE::fts_close(fts), 0);
}

TEST_F(LlvmLibcFtsTest, SkipLeavesOutTheContents) {
  Tree tree(libc_make_test_file_path("fts_skip"));
  char *paths[] = {tree.root, nullptr};
  FTS *fts = LIBC_NAMESPACE::fts_open(paths, FTS_PHYSICAL, by_name);
  ASSERT_TRUE(fts != nullptr);
  Seen seen;
  while (FTSENT *entry = LIBC_NAMESPACE::fts_read(fts)) {
    seen.add(entry);
    if (entry->fts_info == FTS_D &&
        LIBC_NAMESPACE::strcmp(entry->fts_name, "inner") == 0)
      EXPECT_EQ(LIBC_NAMESPACE::fts_set(fts, entry, FTS_SKIP), 0);
  }
  EXPECT_EQ(seen.count, 6);
  EXPECT_TRUE(seen.is(1, FTS_D, "inner"));
  EXPECT_TRUE(seen.is(2, FTS_DP, "inner"));
  EXPECT_EQ(LIBC_NAMESPACE::fts_close(fts), 0);
}

TEST_F(LlvmLibcFtsTest, AgainReturnsTheEntryTwice) {
  Tree tree(libc_make_test_file_path("fts_again"));
  char *paths[] = {const_cast<char *>(tree.top), nullptr};
  FTS *fts = LIBC_NAMESPACE::fts_open(paths, FTS_PHYSICAL, nullptr);
  ASSERT_TRUE(fts != nullptr);
  FTSENT *first = LIBC_NAMESPACE::fts_read(fts);
  ASSERT_TRUE(first != nullptr);
  EXPECT_EQ(LIBC_NAMESPACE::fts_set(fts, first, FTS_AGAIN), 0);
  FTSENT *second = LIBC_NAMESPACE::fts_read(fts);
  EXPECT_EQ(second, first);
  EXPECT_TRUE(LIBC_NAMESPACE::fts_read(fts) == nullptr);
  EXPECT_EQ(LIBC_NAMESPACE::fts_close(fts), 0);
}

TEST_F(LlvmLibcFtsTest, ChildrenListsTheCurrentDirectory) {
  Tree tree(libc_make_test_file_path("fts_children"));
  char *paths[] = {tree.root, nullptr};
  FTS *fts = LIBC_NAMESPACE::fts_open(paths, FTS_PHYSICAL, by_name);
  ASSERT_TRUE(fts != nullptr);
  FTSENT *root = LIBC_NAMESPACE::fts_read(fts);
  ASSERT_TRUE(root != nullptr);
  FTSENT *child = LIBC_NAMESPACE::fts_children(fts, 0);
  ASSERT_TRUE(child != nullptr);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(child->fts_name, "inner"), 0);
  EXPECT_EQ(int(child->fts_info), FTS_D);
  // The walk then carries on with what was listed.
  FTSENT *next = LIBC_NAMESPACE::fts_read(fts);
  EXPECT_EQ(next, child);
  EXPECT_EQ(LIBC_NAMESPACE::fts_close(fts), 0);
}

TEST_F(LlvmLibcFtsTest, NameOnlyChildrenAreNotStatted) {
  Tree tree(libc_make_test_file_path("fts_names"));
  char *paths[] = {tree.root, nullptr};
  FTS *fts = LIBC_NAMESPACE::fts_open(paths, FTS_PHYSICAL, by_name);
  ASSERT_TRUE(fts != nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::fts_read(fts) != nullptr);
  FTSENT *child = LIBC_NAMESPACE::fts_children(fts, FTS_NAMEONLY);
  ASSERT_TRUE(child != nullptr);
  EXPECT_EQ(int(child->fts_info), FTS_NSOK);
  EXPECT_EQ(LIBC_NAMESPACE::fts_close(fts), 0);
}

TEST_F(LlvmLibcFtsTest, LogicalWalkFollowsLinks) {
  Tree tree(libc_make_test_file_path("fts_logical"));
  char *paths[] = {tree.root, nullptr};
  FTS *fts = LIBC_NAMESPACE::fts_open(paths, FTS_LOGICAL, by_name);
  ASSERT_TRUE(fts != nullptr);
  Seen seen;
  while (FTSENT *entry = LIBC_NAMESPACE::fts_read(fts))
    seen.add(entry);
  EXPECT_TRUE(seen.is(4, FTS_F, "link"));
  EXPECT_EQ(LIBC_NAMESPACE::fts_close(fts), 0);
}

TEST_F(LlvmLibcFtsTest, DanglingLinkIsReportedAsSuch) {
  Tree tree(libc_make_test_file_path("fts_dangling"));
  char dangling[4096];
  Tree::join(tree.root, "/dangling", dangling);
  LIBC_NAMESPACE::symlink("no_such_target", dangling);
  char *paths[] = {tree.root, nullptr};
  FTS *fts = LIBC_NAMESPACE::fts_open(paths, FTS_LOGICAL, by_name);
  ASSERT_TRUE(fts != nullptr);
  Seen seen;
  while (FTSENT *entry = LIBC_NAMESPACE::fts_read(fts))
    seen.add(entry);
  EXPECT_TRUE(seen.is(1, FTS_SLNONE, "dangling"));
  EXPECT_EQ(LIBC_NAMESPACE::fts_close(fts), 0);
  LIBC_NAMESPACE::unlink(dangling);
}

TEST_F(LlvmLibcFtsTest, MissingRootIsNotStatted) {
  char name[] = "fts_no_such_root";
  char *paths[] = {name, nullptr};
  FTS *fts = LIBC_NAMESPACE::fts_open(paths, FTS_PHYSICAL, nullptr);
  ASSERT_TRUE(fts != nullptr);
  FTSENT *entry = LIBC_NAMESPACE::fts_read(fts);
  ASSERT_TRUE(entry != nullptr);
  EXPECT_EQ(int(entry->fts_info), FTS_NS);
  EXPECT_EQ(entry->fts_errno, ENOENT);
  EXPECT_EQ(LIBC_NAMESPACE::fts_close(fts), 0);
}

TEST_F(LlvmLibcFtsTest, OnlyKnownOptionsAndInstructionsAreAccepted) {
  char name[] = ".";
  char *paths[] = {name, nullptr};
  EXPECT_TRUE(LIBC_NAMESPACE::fts_open(paths, 0x4000, nullptr) == nullptr);
  ASSERT_ERRNO_EQ(EINVAL);
  libc_errno = 0;

  FTS *fts = LIBC_NAMESPACE::fts_open(paths, FTS_PHYSICAL | FTS_NOSTAT, nullptr);
  ASSERT_TRUE(fts != nullptr);
  FTSENT *entry = LIBC_NAMESPACE::fts_read(fts);
  ASSERT_TRUE(entry != nullptr);
  EXPECT_EQ(LIBC_NAMESPACE::fts_set(fts, entry, 99), 1);
  ASSERT_ERRNO_EQ(EINVAL);
  EXPECT_EQ(LIBC_NAMESPACE::fts_close(fts), 0);
}

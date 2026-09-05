//===-- Unittests for ftw and nftw ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/ftw_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_FTW.h"
#include "hdr/types/struct_stat.h"
#include "src/fcntl/open.h"
#include "src/ftw/ftw.h"
#include "src/ftw/nftw.h"
#include "src/stdio/remove.h"
#include "src/string/strstr.h"
#include "src/sys/stat/mkdir.h"
#include "src/unistd/close.h"
#include "src/unistd/rmdir.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcNftwTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {

// What each callback records, since a callback cannot be given a context.
int files_seen;
int dirs_seen;
int unreadable_seen;
int deepest_level;
int last_base;
bool saw_post_order_before_contents;
int post_order_position;
int entry_position;

void reset() {
  files_seen = 0;
  dirs_seen = 0;
  unreadable_seen = 0;
  deepest_level = 0;
  last_base = -1;
  saw_post_order_before_contents = false;
  post_order_position = -1;
  entry_position = 0;
}

int count_entries(const char *, const struct stat *, int kind,
                  struct FTW *position) {
  ++entry_position;
  if (kind == FTW_F)
    ++files_seen;
  else if (kind == FTW_D || kind == FTW_DP)
    ++dirs_seen;
  else if (kind == FTW_DNR)
    ++unreadable_seen;
  if (position->level > deepest_level)
    deepest_level = position->level;
  last_base = position->base;
  return 0;
}

int count_entries_simple(const char *, const struct stat *, int kind) {
  if (kind == FTW_F)
    ++files_seen;
  else if (kind == FTW_D)
    ++dirs_seen;
  return 0;
}

int stop_at_first_file(const char *, const struct stat *, int kind,
                       struct FTW *) {
  if (kind == FTW_F)
    return 77;
  return 0;
}

int skip_the_subtree(const char *path, const struct stat *, int kind,
                     struct FTW *) {
  if (kind == FTW_D && LIBC_NAMESPACE::strstr(path, "/inner") != nullptr)
    return FTW_SKIP_SUBTREE;
  if (kind == FTW_F)
    ++files_seen;
  return FTW_CONTINUE;
}

int note_root_post_order(const char *path, const struct stat *, int kind,
                         struct FTW *position) {
  ++entry_position;
  if (kind == FTW_DP && position->level == 0)
    post_order_position = entry_position;
  return 0;
}

// The tree every test walks:
//   root/
//     top.txt
//     inner/
//       deep.txt
struct Tree {
  const char *root;

  explicit Tree(const char *r) : root(r) {
    LIBC_NAMESPACE::mkdir(root, S_IRWXU);
    make_file("/top.txt");
    make_dir("/inner");
    make_file("/inner/deep.txt");
  }

  ~Tree() {
    remove_path("/inner/deep.txt");
    remove_path("/inner");
    remove_path("/top.txt");
    LIBC_NAMESPACE::rmdir(root);
  }

  void path_for(const char *tail, char *out) const {
    size_t i = 0;
    for (; root[i] != '\0'; ++i)
      out[i] = root[i];
    size_t j = 0;
    for (; tail[j] != '\0'; ++j)
      out[i + j] = tail[j];
    out[i + j] = '\0';
  }

  void make_file(const char *tail) const {
    char path[256];
    path_for(tail, path);
    int fd = LIBC_NAMESPACE::open(path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd >= 0)
      LIBC_NAMESPACE::close(fd);
  }

  void make_dir(const char *tail) const {
    char path[256];
    path_for(tail, path);
    LIBC_NAMESPACE::mkdir(path, S_IRWXU);
  }

  void remove_path(const char *tail) const {
    char path[256];
    path_for(tail, path);
    LIBC_NAMESPACE::remove(path);
  }
};

} // anonymous namespace

TEST_F(LlvmLibcNftwTest, VisitsEverything) {
  Tree tree(libc_make_test_file_path("nftw_all"));
  reset();
  ASSERT_EQ(LIBC_NAMESPACE::nftw(tree.root, count_entries, 10, 0), 0);
  EXPECT_EQ(files_seen, 2);
  EXPECT_EQ(dirs_seen, 2);
  // The root is level zero, its contents level one, and theirs level two.
  EXPECT_EQ(deepest_level, 2);
}

TEST_F(LlvmLibcNftwTest, FtwTakesTheSimplerCallback) {
  Tree tree(libc_make_test_file_path("nftw_simple"));
  reset();
  ASSERT_EQ(LIBC_NAMESPACE::ftw(tree.root, count_entries_simple, 10), 0);
  EXPECT_EQ(files_seen, 2);
  EXPECT_EQ(dirs_seen, 2);
}

TEST_F(LlvmLibcNftwTest, BaseNamesTheLastComponent) {
  Tree tree(libc_make_test_file_path("nftw_base"));
  reset();
  ASSERT_EQ(LIBC_NAMESPACE::nftw(tree.root, count_entries, 10, FTW_PHYS), 0);
  // Whatever came last, its base has to point past a separator.
  ASSERT_GT(last_base, 0);
}

TEST_F(LlvmLibcNftwTest, CallbackEndsTheWalkWithItsOwnValue) {
  Tree tree(libc_make_test_file_path("nftw_stop"));
  reset();
  EXPECT_EQ(LIBC_NAMESPACE::nftw(tree.root, stop_at_first_file, 10, FTW_PHYS),
            77);
}

TEST_F(LlvmLibcNftwTest, SkipSubtreeLeavesADirectoryUnwalked) {
  Tree tree(libc_make_test_file_path("nftw_skip"));
  reset();
  ASSERT_EQ(LIBC_NAMESPACE::nftw(tree.root, skip_the_subtree, 10,
                                 FTW_PHYS | FTW_ACTIONRETVAL),
            0);
  // Only the file outside the skipped directory.
  EXPECT_EQ(files_seen, 1);
}

TEST_F(LlvmLibcNftwTest, DepthReportsADirectoryAfterItsContents) {
  Tree tree(libc_make_test_file_path("nftw_depth"));
  reset();
  ASSERT_EQ(LIBC_NAMESPACE::nftw(tree.root, note_root_post_order, 10,
                                 FTW_PHYS | FTW_DEPTH),
            0);
  // Four entries, and the root has to be the last of them.
  EXPECT_EQ(post_order_position, 4);
}

TEST_F(LlvmLibcNftwTest, OneDescriptorIsEnough) {
  Tree tree(libc_make_test_file_path("nftw_onefd"));
  reset();
  // With only one directory allowed open at a time the walk has to close and
  // reopen as it descends, and still see everything.
  ASSERT_EQ(LIBC_NAMESPACE::nftw(tree.root, count_entries, 1, FTW_PHYS), 0);
  EXPECT_EQ(files_seen, 2);
  EXPECT_EQ(dirs_seen, 2);
}

TEST_F(LlvmLibcNftwTest, RejectsBadArguments) {
  Tree tree(libc_make_test_file_path("nftw_bad"));
  EXPECT_EQ(LIBC_NAMESPACE::nftw(tree.root, count_entries, 0, 0), -1);
  ASSERT_ERRNO_EQ(EINVAL);
  EXPECT_EQ(LIBC_NAMESPACE::nftw(nullptr, count_entries, 10, 0), -1);
  ASSERT_ERRNO_EQ(EINVAL);
}

TEST_F(LlvmLibcNftwTest, MissingRootIsAnError) {
  EXPECT_EQ(LIBC_NAMESPACE::nftw(libc_make_test_file_path("nftw_absent"),
                                 count_entries, 10, 0),
            -1);
  ASSERT_ERRNO_EQ(ENOENT);
}

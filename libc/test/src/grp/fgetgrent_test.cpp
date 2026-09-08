//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for fgetgrent, putgrent and getgrouplist.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/struct_group.h"
#include "src/__support/CPP/string_view.h"
#include "src/fcntl/open.h"
#include "src/grp/fgetgrent.h"
#include "src/grp/getgrouplist.h"
#include "src/grp/grp_utils.h"
#include "src/grp/putgrent.h"
#include "src/stdio/fclose.h"
#include "src/stdio/fopen.h"
#include "src/stdio/remove.h"
#include "src/unistd/close.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcFGetGrEntTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

namespace {

constexpr const char *GROUP_FILE = "fgetgrent.test.group";
constexpr const char *CONTENTS = "root:x:0:\n"
                                 "daemon:x:1:andy\n"
                                 "wheel:x:10:root,andy,jo\n";

void write_group_file() {
  int fd =
      LIBC_NAMESPACE::open(GROUP_FILE, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  string_view contents(CONTENTS);
  ASSERT_EQ(LIBC_NAMESPACE::write(fd, contents.data(), contents.size()),
            static_cast<ssize_t>(contents.size()));
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);
  LIBC_NAMESPACE::group::TESTONLY_set_group_path(GROUP_FILE);
}

} // anonymous namespace

// fgetgrent reads from a stream the caller opened rather than from the
// group file the rest of the family reads.
TEST_F(LlvmLibcFGetGrEntTest, ReadsEveryEntryFromAStream) {
  write_group_file();

  ::FILE *f = LIBC_NAMESPACE::fopen(GROUP_FILE, "r");
  ASSERT_TRUE(f != nullptr);

  struct group *g = LIBC_NAMESPACE::fgetgrent(f);
  ASSERT_TRUE(g != nullptr);
  EXPECT_STREQ(g->gr_name, "root");
  EXPECT_TRUE(g->gr_mem[0] == nullptr);

  g = LIBC_NAMESPACE::fgetgrent(f);
  ASSERT_TRUE(g != nullptr);
  EXPECT_STREQ(g->gr_name, "daemon");
  EXPECT_STREQ(g->gr_mem[0], "andy");

  g = LIBC_NAMESPACE::fgetgrent(f);
  ASSERT_TRUE(g != nullptr);
  EXPECT_STREQ(g->gr_name, "wheel");
  EXPECT_EQ(g->gr_gid, 10u);

  EXPECT_TRUE(LIBC_NAMESPACE::fgetgrent(f) == nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
  ASSERT_EQ(LIBC_NAMESPACE::remove(GROUP_FILE), 0);
}

// What putgrent writes reads back as the entry it was given.
TEST_F(LlvmLibcFGetGrEntTest, WritesAnEntryOut) {
  constexpr const char *OUT_FILE = "putgrent.test.group";

  char name[] = "wheel";
  char password[] = "x";
  char first[] = "root";
  char second[] = "andy";
  char *members[] = {first, second, nullptr};
  struct group g = {};
  g.gr_name = name;
  g.gr_passwd = password;
  g.gr_gid = 10;
  g.gr_mem = members;

  ::FILE *out = LIBC_NAMESPACE::fopen(OUT_FILE, "w");
  ASSERT_TRUE(out != nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::putgrent(&g, out), 0);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(out), 0);

  ::FILE *in = LIBC_NAMESPACE::fopen(OUT_FILE, "r");
  ASSERT_TRUE(in != nullptr);
  struct group *read = LIBC_NAMESPACE::fgetgrent(in);
  ASSERT_TRUE(read != nullptr);
  EXPECT_STREQ(read->gr_name, "wheel");
  EXPECT_EQ(read->gr_gid, 10u);
  EXPECT_STREQ(read->gr_mem[0], "root");
  EXPECT_STREQ(read->gr_mem[1], "andy");
  EXPECT_TRUE(read->gr_mem[2] == nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(in), 0);

  ASSERT_EQ(LIBC_NAMESPACE::remove(OUT_FILE), 0);
}

// getgrouplist reports every group a user is in, the one given first, and
// says how many there were whether or not they fitted.
TEST_F(LlvmLibcFGetGrEntTest, ListsTheGroupsAUserIsIn) {
  write_group_file();

  gid_t groups[8] = {};
  int count = 8;
  ASSERT_EQ(LIBC_NAMESPACE::getgrouplist("andy", 100, groups, &count), count);
  ASSERT_EQ(count, 3);
  EXPECT_EQ(groups[0], gid_t(100));
  EXPECT_EQ(groups[1], gid_t(1));
  EXPECT_EQ(groups[2], gid_t(10));

  // Too little room is reported by returning -1, with the count set to what
  // was needed rather than to what was written.
  int small = 1;
  gid_t one[1] = {};
  ASSERT_EQ(LIBC_NAMESPACE::getgrouplist("andy", 100, one, &small), -1);
  ASSERT_EQ(small, 3);

  // A user in no group at all is still in the one it was given.
  int alone = 8;
  ASSERT_EQ(LIBC_NAMESPACE::getgrouplist("nobody", 42, groups, &alone), alone);
  ASSERT_EQ(alone, 1);
  EXPECT_EQ(groups[0], gid_t(42));

  ASSERT_EQ(LIBC_NAMESPACE::remove(GROUP_FILE), 0);
}

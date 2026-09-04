//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for getgrnam, getgrgid and the getgrent iteration.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_group.h"
#include "src/__support/CPP/string_view.h"
#include "src/fcntl/open.h"
#include "src/grp/endgrent.h"
#include "src/grp/getgrent.h"
#include "src/grp/getgrgid.h"
#include "src/grp/getgrnam.h"
#include "src/grp/grp_utils.h"
#include "src/grp/setgrent.h"
#include "src/stdio/remove.h"
#include "src/unistd/close.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcGetGrNamTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

namespace {

constexpr const char *GROUP_FILE = "getgrnam.test.group";
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

TEST_F(LlvmLibcGetGrNamTest, FindsByName) {
  write_group_file();

  struct group *g = LIBC_NAMESPACE::getgrnam("wheel");
  ASSERT_TRUE(g != nullptr);
  EXPECT_STREQ(g->gr_name, "wheel");
  EXPECT_EQ(g->gr_gid, 10u);
  EXPECT_STREQ(g->gr_mem[0], "root");
  EXPECT_STREQ(g->gr_mem[1], "andy");
  EXPECT_STREQ(g->gr_mem[2], "jo");
  EXPECT_TRUE(g->gr_mem[3] == nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::remove(GROUP_FILE), 0);
}

TEST_F(LlvmLibcGetGrNamTest, FindsByGid) {
  write_group_file();

  struct group *g = LIBC_NAMESPACE::getgrgid(1);
  ASSERT_TRUE(g != nullptr);
  EXPECT_STREQ(g->gr_name, "daemon");
  EXPECT_STREQ(g->gr_mem[0], "andy");
  EXPECT_TRUE(g->gr_mem[1] == nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::remove(GROUP_FILE), 0);
}

TEST_F(LlvmLibcGetGrNamTest, MissingNameIsNotAnError) {
  write_group_file();

  EXPECT_TRUE(LIBC_NAMESPACE::getgrnam("no-such-group") == nullptr);
  ASSERT_ERRNO_SUCCESS();
  EXPECT_TRUE(LIBC_NAMESPACE::getgrgid(4242) == nullptr);
  ASSERT_ERRNO_SUCCESS();

  ASSERT_EQ(LIBC_NAMESPACE::remove(GROUP_FILE), 0);
}

TEST_F(LlvmLibcGetGrNamTest, NullNameFails) {
  write_group_file();

  EXPECT_TRUE(LIBC_NAMESPACE::getgrnam(nullptr) == nullptr);
  ASSERT_ERRNO_EQ(EINVAL);

  ASSERT_EQ(LIBC_NAMESPACE::remove(GROUP_FILE), 0);
}

TEST_F(LlvmLibcGetGrNamTest, IterationWalksEveryEntry) {
  write_group_file();

  LIBC_NAMESPACE::setgrent();
  int count = 0;
  while (struct group *g = LIBC_NAMESPACE::getgrent()) {
    ASSERT_TRUE(g->gr_name != nullptr);
    ++count;
  }
  EXPECT_EQ(count, 3);
  LIBC_NAMESPACE::endgrent();

  ASSERT_EQ(LIBC_NAMESPACE::remove(GROUP_FILE), 0);
}

TEST_F(LlvmLibcGetGrNamTest, LookupResetsAnIteration) {
  write_group_file();

  LIBC_NAMESPACE::setgrent();
  struct group *first = LIBC_NAMESPACE::getgrent();
  ASSERT_TRUE(first != nullptr);
  EXPECT_STREQ(first->gr_name, "root");

  // A lookup shares the one handle, so it rewinds the iteration rather than
  // leaving it where it was.
  ASSERT_TRUE(LIBC_NAMESPACE::getgrnam("wheel") != nullptr);

  struct group *next = LIBC_NAMESPACE::getgrent();
  ASSERT_TRUE(next != nullptr);
  EXPECT_STREQ(next->gr_name, "root");
  LIBC_NAMESPACE::endgrent();

  ASSERT_EQ(LIBC_NAMESPACE::remove(GROUP_FILE), 0);
}

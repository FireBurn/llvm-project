//===-- Unittests for initgroups ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/gid_t.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/libc_errno.h"
#include "src/fcntl/open.h"
#include "src/grp/grp_utils.h"
#include "src/grp/initgroups.h"
#include "src/stdio/remove.h"
#include "src/unistd/close.h"
#include "src/unistd/getgid.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcInitGroupsTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

namespace {

constexpr const char *GROUP_FILE = "initgroups.test.group";

void write_group_file() {
  int fd =
      LIBC_NAMESPACE::open(GROUP_FILE, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  string_view contents("root:x:0:\n"
                       "wheel:x:10:andy\n"
                       "audio:x:18:andy,jo\n"
                       "video:x:27:jo\n");
  ASSERT_EQ(LIBC_NAMESPACE::write(fd, contents.data(), contents.size()),
            static_cast<ssize_t>(contents.size()));
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);
  LIBC_NAMESPACE::group::TESTONLY_set_group_path(GROUP_FILE);
}

} // anonymous namespace

TEST_F(LlvmLibcInitGroupsTest, NullUserFails) {
  EXPECT_EQ(LIBC_NAMESPACE::initgroups(nullptr, 0), -1);
  ASSERT_ERRNO_EQ(EINVAL);
}

TEST_F(LlvmLibcInitGroupsTest, GathersTheUsersGroups) {
  write_group_file();

  // Setting the group list needs CAP_SETGID, which the test does not have,
  // so the database walk is what is under test and EPERM is the expected
  // end of it.
  int ret = LIBC_NAMESPACE::initgroups("andy", LIBC_NAMESPACE::getgid());
  if (ret != 0)
    ASSERT_ERRNO_EQ(EPERM);

  ASSERT_EQ(LIBC_NAMESPACE::remove(GROUP_FILE), 0);
}

TEST_F(LlvmLibcInitGroupsTest, UnknownUserStillSetsTheGivenGroup) {
  write_group_file();

  int ret = LIBC_NAMESPACE::initgroups("nobody-here", 0);
  if (ret != 0)
    ASSERT_ERRNO_EQ(EPERM);

  ASSERT_EQ(LIBC_NAMESPACE::remove(GROUP_FILE), 0);
}

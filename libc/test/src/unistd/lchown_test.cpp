//===-- Unittests for lchown ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/fcntl/open.h"
#include "src/sys/stat/lstat.h"
#include "src/unistd/close.h"
#include "src/unistd/getegid.h"
#include "src/unistd/geteuid.h"
#include "src/unistd/lchown.h"
#include "src/unistd/symlink.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcLchownTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// lchown acts on the link itself rather than on what it points at, so the
// owner of the target is left alone.
TEST_F(LlvmLibcLchownTest, ChangesTheLinkAndNotTheTarget) {
  auto target = libc_make_test_file_path("lchown_target.test");
  auto link = libc_make_test_file_path("lchown_link.test");

  int fd = LIBC_NAMESPACE::open(target, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::symlink(target, link), Succeeds(0));

  // Asking for the owner already held is allowed whoever is asking.
  uid_t uid = LIBC_NAMESPACE::geteuid();
  gid_t gid = LIBC_NAMESPACE::getegid();
  ASSERT_THAT(LIBC_NAMESPACE::lchown(link, uid, gid), Succeeds(0));

  struct stat st;
  ASSERT_THAT(LIBC_NAMESPACE::lstat(link, &st), Succeeds(0));
  ASSERT_EQ(st.st_uid, uid);
  ASSERT_EQ(st.st_gid, gid);

  ASSERT_THAT(LIBC_NAMESPACE::unlink(link), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(target), Succeeds(0));
}

TEST_F(LlvmLibcLchownTest, PathThatIsNotThere) {
  ASSERT_THAT(LIBC_NAMESPACE::lchown("/nonexistent/llvm_libc", 0, 0),
              Fails(ENOENT));
}

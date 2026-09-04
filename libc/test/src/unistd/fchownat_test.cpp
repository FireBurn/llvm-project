//===-- Unittests for fchownat --------------------------------------------===//
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
#include "src/sys/stat/stat.h"
#include "src/unistd/close.h"
#include "src/unistd/fchownat.h"
#include "src/unistd/getegid.h"
#include "src/unistd/geteuid.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcFchownatTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// Asking for the owner already held is allowed whoever is asking, which is
// the one case a test that is not root can make.
TEST_F(LlvmLibcFchownatTest, RelativeToTheWorkingDirectory) {
  auto path = libc_make_test_file_path("fchownat.test");
  int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));

  uid_t uid = LIBC_NAMESPACE::geteuid();
  gid_t gid = LIBC_NAMESPACE::getegid();
  ASSERT_THAT(LIBC_NAMESPACE::fchownat(AT_FDCWD, path, uid, gid, 0),
              Succeeds(0));

  struct stat st;
  ASSERT_THAT(LIBC_NAMESPACE::stat(path, &st), Succeeds(0));
  ASSERT_EQ(st.st_uid, uid);
  ASSERT_EQ(st.st_gid, gid);

  // A value of -1 for either leaves that one as it was.
  ASSERT_THAT(LIBC_NAMESPACE::fchownat(AT_FDCWD, path, static_cast<uid_t>(-1),
                                       static_cast<gid_t>(-1), 0),
              Succeeds(0));

  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcFchownatTest, PathThatIsNotThere) {
  ASSERT_THAT(LIBC_NAMESPACE::fchownat(AT_FDCWD, "llvm-libc-not-here", 0, 0, 0),
              Fails(ENOENT));
}

TEST_F(LlvmLibcFchownatTest, DirectoryDescriptorThatIsNotOne) {
  ASSERT_THAT(LIBC_NAMESPACE::fchownat(-1, "llvm-libc-not-here", 0, 0, 0),
              Fails(EBADF));
}

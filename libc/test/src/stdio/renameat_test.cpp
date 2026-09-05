//===-- Unittests for renameat and renameat2 ------------------------------===//
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
#include "src/stdio/renameat.h"
#include "src/stdio/renameat2.h"
#include "src/sys/stat/stat.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include <linux/fs.h>

using LlvmLibcRenameatTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

namespace {

int make_file(const char *path) {
  int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  if (fd > 0)
    LIBC_NAMESPACE::close(fd);
  return fd;
}

} // anonymous namespace

TEST_F(LlvmLibcRenameatTest, MovesAFileWithinTheWorkingDirectory) {
  auto from = libc_make_test_file_path("renameat_from.test");
  auto to = libc_make_test_file_path("renameat_to.test");
  ASSERT_GT(make_file(from), 0);

  ASSERT_THAT(LIBC_NAMESPACE::renameat(AT_FDCWD, from, AT_FDCWD, to),
              Succeeds(0));

  struct stat st;
  ASSERT_THAT(LIBC_NAMESPACE::stat(to, &st), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::stat(from, &st), Fails(ENOENT));

  ASSERT_THAT(LIBC_NAMESPACE::unlink(to), Succeeds(0));
}

// RENAME_NOREPLACE refuses to write over a name that is taken, which is
// what separates renameat2 from renameat.
TEST_F(LlvmLibcRenameatTest, RefusesToReplaceWhenAskedNotTo) {
  auto from = libc_make_test_file_path("renameat2_from.test");
  auto to = libc_make_test_file_path("renameat2_to.test");
  ASSERT_GT(make_file(from), 0);
  ASSERT_GT(make_file(to), 0);

  ASSERT_THAT(
      LIBC_NAMESPACE::renameat2(AT_FDCWD, from, AT_FDCWD, to, RENAME_NOREPLACE),
      Fails(EEXIST));

  // Without the flag it goes over the top of it.
  ASSERT_THAT(LIBC_NAMESPACE::renameat2(AT_FDCWD, from, AT_FDCWD, to, 0),
              Succeeds(0));

  struct stat st;
  ASSERT_THAT(LIBC_NAMESPACE::stat(from, &st), Fails(ENOENT));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(to), Succeeds(0));
}

TEST_F(LlvmLibcRenameatTest, PathThatIsNotThere) {
  ASSERT_THAT(LIBC_NAMESPACE::renameat(AT_FDCWD, "llvm-libc-not-here", AT_FDCWD,
                                       "llvm-libc-elsewhere"),
              Fails(ENOENT));
}

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for renameat2.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/unistd_macros.h"
#include "src/__support/CPP/scope.h"
#include "src/__support/libc_errno.h"
#include "src/fcntl/open.h"
#include "src/stdio/renameat2.h"
#include "src/unistd/access.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include <linux/fs.h>

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcRenameat2Test = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

static int make_file(const char *path) {
  int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT, S_IRWXU);
  if (fd < 0)
    return fd;
  return LIBC_NAMESPACE::close(fd);
}

// RENAME_NOREPLACE refuses to write over a name that is taken, which is what
// separates renameat2 from renameat.
TEST_F(LlvmLibcRenameat2Test, RefusesToReplaceWhenAskedNotTo) {
  auto FROM = libc_make_test_file_path("renameat2.test.from");
  auto TO = libc_make_test_file_path("renameat2.test.to");
  ASSERT_THAT(make_file(FROM), Succeeds(0));
  ASSERT_THAT(make_file(TO), Succeeds(0));
  LIBC_NAMESPACE::cpp::scope_exit cleanup_files([&] {
    LIBC_NAMESPACE::unlink(FROM);
    LIBC_NAMESPACE::unlink(TO);
    LIBC_NAMESPACE::libc_errno = 0;
  });

  ASSERT_THAT(
      LIBC_NAMESPACE::renameat2(AT_FDCWD, FROM, AT_FDCWD, TO, RENAME_NOREPLACE),
      Fails(EEXIST));

  // Without the flag it goes over the top of it.
  ASSERT_THAT(LIBC_NAMESPACE::renameat2(AT_FDCWD, FROM, AT_FDCWD, TO, 0),
              Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::access(FROM, F_OK), Fails(ENOENT));
  ASSERT_THAT(LIBC_NAMESPACE::access(TO, F_OK), Succeeds(0));
}

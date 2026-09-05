//===-- Unittests for mkfifo and mkfifoat ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/sys/stat/mkfifo.h"
#include "src/sys/stat/mkfifoat.h"
#include "src/sys/stat/stat.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcMkfifoTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

TEST_F(LlvmLibcMkfifoTest, MakesAFifo) {
  auto path = libc_make_test_file_path("mkfifo.test");
  ASSERT_THAT(LIBC_NAMESPACE::mkfifo(path, S_IRWXU), Succeeds(0));

  struct stat st;
  ASSERT_THAT(LIBC_NAMESPACE::stat(path, &st), Succeeds(0));
  ASSERT_TRUE(S_ISFIFO(st.st_mode));

  // A name that is already taken is refused.
  ASSERT_THAT(LIBC_NAMESPACE::mkfifo(path, S_IRWXU), Fails(EEXIST));

  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcMkfifoTest, MakesAFifoRelativeToTheWorkingDirectory) {
  auto path = libc_make_test_file_path("mkfifoat.test");
  ASSERT_THAT(LIBC_NAMESPACE::mkfifoat(AT_FDCWD, path, S_IRWXU), Succeeds(0));

  struct stat st;
  ASSERT_THAT(LIBC_NAMESPACE::stat(path, &st), Succeeds(0));
  ASSERT_TRUE(S_ISFIFO(st.st_mode));

  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcMkfifoTest, DirectoryThatIsNotThere) {
  ASSERT_THAT(LIBC_NAMESPACE::mkfifo("/nonexistent/llvm_libc", S_IRWXU),
              Fails(ENOENT));
  ASSERT_THAT(LIBC_NAMESPACE::mkfifoat(-1, "llvm-libc-not-here", S_IRWXU),
              Fails(EBADF));
}

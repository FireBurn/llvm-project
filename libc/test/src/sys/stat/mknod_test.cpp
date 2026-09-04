//===-- Unittests for mknod and mknodat -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_stat.h"
#include "src/stdio/remove.h"
#include "src/sys/stat/mknod.h"
#include "src/sys/stat/mknodat.h"
#include "src/sys/stat/stat.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcMknodTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcMknodTest, MakesAFifo) {
  constexpr const char *FIFO = "mknod.test.fifo";
  LIBC_NAMESPACE::remove(FIFO);
  libc_errno = 0;

  // A fifo is the one node an unprivileged process is allowed to make.
  ASSERT_THAT(LIBC_NAMESPACE::mknod(FIFO, S_IFIFO | S_IRUSR | S_IWUSR, 0),
              Succeeds(0));

  struct stat buf;
  ASSERT_THAT(LIBC_NAMESPACE::stat(FIFO, &buf), Succeeds(0));
  EXPECT_TRUE(S_ISFIFO(buf.st_mode));

  ASSERT_EQ(LIBC_NAMESPACE::remove(FIFO), 0);
}

TEST_F(LlvmLibcMknodTest, MakesARegularFile) {
  constexpr const char *FILE = "mknod.test.reg";
  LIBC_NAMESPACE::remove(FILE);
  libc_errno = 0;

  // A mode with no type in it means a regular file.
  ASSERT_THAT(LIBC_NAMESPACE::mknod(FILE, S_IRUSR | S_IWUSR, 0), Succeeds(0));

  struct stat buf;
  ASSERT_THAT(LIBC_NAMESPACE::stat(FILE, &buf), Succeeds(0));
  EXPECT_TRUE(S_ISREG(buf.st_mode));

  ASSERT_EQ(LIBC_NAMESPACE::remove(FILE), 0);
}

TEST_F(LlvmLibcMknodTest, RelativeToTheWorkingDirectory) {
  constexpr const char *FIFO = "mknodat.test.fifo";
  LIBC_NAMESPACE::remove(FIFO);
  libc_errno = 0;

  ASSERT_THAT(LIBC_NAMESPACE::mknodat(AT_FDCWD, FIFO, S_IFIFO | S_IRUSR, 0),
              Succeeds(0));

  struct stat buf;
  ASSERT_THAT(LIBC_NAMESPACE::stat(FIFO, &buf), Succeeds(0));
  EXPECT_TRUE(S_ISFIFO(buf.st_mode));

  ASSERT_EQ(LIBC_NAMESPACE::remove(FIFO), 0);
}

TEST_F(LlvmLibcMknodTest, ExistingPathFails) {
  constexpr const char *FIFO = "mknod.test.twice";
  LIBC_NAMESPACE::remove(FIFO);
  libc_errno = 0;

  ASSERT_THAT(LIBC_NAMESPACE::mknod(FIFO, S_IFIFO | S_IRUSR, 0), Succeeds(0));
  EXPECT_THAT(LIBC_NAMESPACE::mknod(FIFO, S_IFIFO | S_IRUSR, 0),
              Fails(EEXIST, -1));

  ASSERT_EQ(LIBC_NAMESPACE::remove(FIFO), 0);
}

TEST_F(LlvmLibcMknodTest, MissingDirectoryFails) {
  EXPECT_THAT(LIBC_NAMESPACE::mknod("mknod.no.such.dir/x", S_IFIFO, 0),
              Fails(ENOENT, -1));
}

TEST_F(LlvmLibcMknodTest, DeviceNeedsPrivilege) {
  // Making a character device is reserved to a process with CAP_MKNOD, and
  // the test does not have it.
  int ret = LIBC_NAMESPACE::mknod("mknod.test.dev", S_IFCHR | S_IRUSR, 0);
  if (ret == 0) {
    ASSERT_EQ(LIBC_NAMESPACE::remove("mknod.test.dev"), 0);
    return;
  }
  ASSERT_ERRNO_EQ(EPERM);
}

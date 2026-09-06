//===-- Unittests for splice and sync_file_range --------------------------===//
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
#include "src/fcntl/splice.h"
#include "src/fcntl/sync_file_range.h"
#include "src/string/memcmp.h"
#include "src/unistd/close.h"
#include "src/unistd/pipe.h"
#include "src/unistd/read.h"
#include "src/unistd/unlink.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcSpliceTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// splice moves bytes between descriptors without copying them through the
// caller, and one end of it always has to be a pipe.
TEST_F(LlvmLibcSpliceTest, MovesFromAFileIntoAPipe) {
  constexpr char TEXT[] = "splice";
  constexpr size_t SIZE = sizeof(TEXT) - 1;

  auto path = libc_make_test_file_path("splice.test");
  int fd = LIBC_NAMESPACE::open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::write(fd, TEXT, SIZE), Succeeds<ssize_t>(SIZE));

  int fds[2];
  ASSERT_THAT(LIBC_NAMESPACE::pipe(fds), Succeeds(0));

  off_t offset = 0;
  ASSERT_THAT(LIBC_NAMESPACE::splice(fd, &offset, fds[1], nullptr, SIZE, 0),
              Succeeds<ssize_t>(SIZE));
  ASSERT_EQ(offset, static_cast<off_t>(SIZE));

  char buf[SIZE];
  ASSERT_THAT(LIBC_NAMESPACE::read(fds[0], buf, SIZE), Succeeds<ssize_t>(SIZE));
  ASSERT_EQ(LIBC_NAMESPACE::memcmp(buf, TEXT, SIZE), 0);

  ASSERT_THAT(LIBC_NAMESPACE::close(fds[0]), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(fds[1]), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

// Neither end being a pipe is what splice cannot do, and it says so.
TEST_F(LlvmLibcSpliceTest, NeitherEndIsAPipe) {
  auto first = libc_make_test_file_path("splice_first.test");
  auto second = libc_make_test_file_path("splice_second.test");
  int in = LIBC_NAMESPACE::open(first, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(in, 0);
  int out = LIBC_NAMESPACE::open(second, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(out, 0);

  ASSERT_THAT(LIBC_NAMESPACE::splice(in, nullptr, out, nullptr, 1, 0),
              Fails<ssize_t>(EINVAL));

  ASSERT_THAT(LIBC_NAMESPACE::close(in), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(out), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(first), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(second), Succeeds(0));
}

TEST_F(LlvmLibcSpliceTest, WritingBackPartOfAFile) {
  constexpr char TEXT[] = "sync_file_range";
  auto path = libc_make_test_file_path("sync_file_range.test");
  int fd = LIBC_NAMESPACE::open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);
  ASSERT_THAT(LIBC_NAMESPACE::write(fd, TEXT, sizeof(TEXT)),
              Succeeds<ssize_t>(sizeof(TEXT)));

  ASSERT_THAT(LIBC_NAMESPACE::sync_file_range(fd, 0, 0, 0), Succeeds(0));

  ASSERT_THAT(LIBC_NAMESPACE::sync_file_range(-1, 0, 0, 0), Fails(EBADF));
  ASSERT_THAT(LIBC_NAMESPACE::sync_file_range(fd, -1, 0, 0), Fails(EINVAL));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

//===-- Unittests for preadv, pwritev and the process_vm calls ------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/stdio_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_iovec.h"
#include "src/__support/libc_errno.h"
#include "src/fcntl/open.h"
#include "src/string/memcmp.h"
#include "src/sys/uio/preadv.h"
#include "src/sys/uio/process_vm_readv.h"
#include "src/sys/uio/process_vm_writev.h"
#include "src/sys/uio/pwritev.h"
#include "src/unistd/close.h"
#include "src/unistd/getpid.h"
#include "src/unistd/lseek.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcPvecTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// The offset is given rather than taken from the descriptor, so the
// descriptor's own offset is left where it was.
TEST_F(LlvmLibcPvecTest, WriteAndReadAtAStatedOffset) {
  auto path = libc_make_test_file_path("pvec.test");
  int fd = LIBC_NAMESPACE::open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);

  char first[] = "abc";
  char second[] = "def";
  struct iovec out[2];
  out[0].iov_base = first;
  out[0].iov_len = 3;
  out[1].iov_base = second;
  out[1].iov_len = 3;

  ASSERT_THAT(LIBC_NAMESPACE::pwritev(fd, out, 2, 4), Succeeds<ssize_t>(6));
  ASSERT_THAT(LIBC_NAMESPACE::lseek(fd, 0, SEEK_CUR), Succeeds<off_t>(0));

  char left[3] = {};
  char right[3] = {};
  struct iovec in[2];
  in[0].iov_base = left;
  in[0].iov_len = 3;
  in[1].iov_base = right;
  in[1].iov_len = 3;

  ASSERT_THAT(LIBC_NAMESPACE::preadv(fd, in, 2, 4), Succeeds<ssize_t>(6));
  ASSERT_EQ(LIBC_NAMESPACE::memcmp(left, "abc", 3), 0);
  ASSERT_EQ(LIBC_NAMESPACE::memcmp(right, "def", 3), 0);

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::unlink(path), Succeeds(0));
}

TEST_F(LlvmLibcPvecTest, DescriptorThatIsNotOne) {
  char buf[4];
  struct iovec iov;
  iov.iov_base = buf;
  iov.iov_len = sizeof(buf);
  ASSERT_THAT(LIBC_NAMESPACE::preadv(-1, &iov, 1, 0), Fails<ssize_t>(EBADF));
  ASSERT_THAT(LIBC_NAMESPACE::pwritev(-1, &iov, 1, 0), Fails<ssize_t>(EBADF));
}

// Reading this process's own memory needs no privilege, so it is the one
// case a test can make of the process_vm calls.
TEST_F(LlvmLibcPvecTest, ReadingAndWritingOurOwnMemory) {
  char source[] = "process_vm";
  char destination[sizeof(source)] = {};

  struct iovec local;
  local.iov_base = destination;
  local.iov_len = sizeof(source);
  struct iovec remote;
  remote.iov_base = source;
  remote.iov_len = sizeof(source);

  ssize_t read = LIBC_NAMESPACE::process_vm_readv(LIBC_NAMESPACE::getpid(),
                                                  &local, 1, &remote, 1, 0);
  if (read < 0) {
    // A machine may forbid one process reading another's memory outright,
    // and says so with EPERM whoever is asking.
    ASSERT_ERRNO_EQ(EPERM);
    LIBC_NAMESPACE::libc_errno = 0;
    return;
  }
  ASSERT_EQ(read, static_cast<ssize_t>(sizeof(source)));
  ASSERT_EQ(LIBC_NAMESPACE::memcmp(destination, source, sizeof(source)), 0);

  char written[sizeof(source)] = {};
  local.iov_base = source;
  local.iov_len = sizeof(source);
  remote.iov_base = written;
  remote.iov_len = sizeof(source);
  ASSERT_THAT(LIBC_NAMESPACE::process_vm_writev(LIBC_NAMESPACE::getpid(),
                                                &local, 1, &remote, 1, 0),
              Succeeds<ssize_t>(sizeof(source)));
  ASSERT_EQ(LIBC_NAMESPACE::memcmp(written, source, sizeof(source)), 0);
}

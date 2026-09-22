//===-- Unittests for syscalls --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/syscall.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h" // For S_* flags.
#include <sys/syscall.h> // For syscall numbers.
#include <unistd.h>

using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;
using LlvmLibcSyscallTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// We only do a smoke test here. Actual functionality tests are
// done by the unit tests of the syscall wrappers like mmap.
// The goal is to test syscalls with a wide number of args.

// The public syscall is variadic and is not in the namespace, so the tests
// call the internal function it forwards to, with the unused arguments zero.
template <typename... Args>
static long call_syscall(long number, Args... args) {
  long a[6] = {(long)args...};
  return LIBC_NAMESPACE::__llvm_libc_syscall(number, a[0], a[1], a[2], a[3],
                                             a[4], a[5]);
}

TEST_F(LlvmLibcSyscallTest, TrivialCall) {
  ASSERT_GE(call_syscall(SYS_gettid), 0l);
  ASSERT_ERRNO_SUCCESS();
}

TEST_F(LlvmLibcSyscallTest, SymlinkCreateDestroy) {
  constexpr const char LINK_VAL[] = "syscall_readlink_test_value";
  constexpr const char LINK[] = "testdata/syscall_readlink.test.link";

#ifdef SYS_symlink
  ASSERT_GE(call_syscall(SYS_symlink, LINK_VAL, LINK), 0l);
#elif defined(SYS_symlinkat)
  ASSERT_GE(call_syscall(SYS_symlinkat, LINK_VAL, AT_FDCWD, LINK), 0l);
#else
#error "symlink and symlinkat syscalls not available."
#endif
  ASSERT_ERRNO_SUCCESS();

  char buf[sizeof(LINK_VAL)];

#ifdef SYS_readlink
  ASSERT_GE(call_syscall(SYS_readlink, LINK, buf, sizeof(buf)), 0l);
#elif defined(SYS_readlinkat)
  ASSERT_GE(call_syscall(SYS_readlinkat, AT_FDCWD, LINK, buf, sizeof(buf)), 0l);
#endif
  ASSERT_ERRNO_SUCCESS();

#ifdef SYS_unlink
  ASSERT_GE(call_syscall(SYS_unlink, LINK), 0l);
#elif defined(SYS_unlinkat)
  ASSERT_GE(call_syscall(SYS_unlinkat, AT_FDCWD, LINK, 0), 0l);
#else
#error "unlink and unlinkat syscalls not available."
#endif
  ASSERT_ERRNO_SUCCESS();
}

TEST_F(LlvmLibcSyscallTest, FileReadWrite) {
  constexpr const char HELLO[] = "hello";
  constexpr int HELLO_SIZE = sizeof(HELLO);

  constexpr const char *TEST_FILE = "testdata/syscall_pread_pwrite.test";

#ifdef SYS_open
  long fd = call_syscall(SYS_open, TEST_FILE, O_WRONLY | O_CREAT, S_IRWXU);
#elif defined(SYS_openat)
  long fd = call_syscall(SYS_openat, AT_FDCWD, TEST_FILE, O_WRONLY | O_CREAT,
                         S_IRWXU);
#else
#error "open and openat syscalls not available."
#endif
  ASSERT_GT(fd, 0l);
  ASSERT_ERRNO_SUCCESS();

  ASSERT_GE(call_syscall(SYS_pwrite64, fd, HELLO, HELLO_SIZE, 0), 0l);
  ASSERT_ERRNO_SUCCESS();

  ASSERT_GE(call_syscall(SYS_fsync, fd), 0l);
  ASSERT_ERRNO_SUCCESS();

  ASSERT_GE(call_syscall(SYS_close, fd), 0l);
  ASSERT_ERRNO_SUCCESS();
}

TEST_F(LlvmLibcSyscallTest, FileLinkCreateDestroy) {
  constexpr const char *TEST_DIR = "testdata";
  constexpr const char *TEST_FILE = "syscall_linkat.test";
  constexpr const char *TEST_FILE_PATH = "testdata/syscall_linkat.test";
  constexpr const char *TEST_FILE_LINK = "syscall_linkat.test.link";
  constexpr const char *TEST_FILE_LINK_PATH =
      "testdata/syscall_linkat.test.link";

  // The test strategy is as follows:
  //   1. Create a normal file
  //   2. Create a link to that file.
  //   3. Open the link to check that the link was created.
  //   4. Cleanup the file and its link.

#ifdef SYS_open
  long write_fd =
      call_syscall(SYS_open, TEST_FILE_PATH, O_WRONLY | O_CREAT, S_IRWXU);
#elif defined(SYS_openat)
  long write_fd = call_syscall(SYS_openat, AT_FDCWD, TEST_FILE_PATH,
                               O_WRONLY | O_CREAT, S_IRWXU);
#else
#error "open and openat syscalls not available."
#endif
  ASSERT_GT(write_fd, 0l);
  ASSERT_ERRNO_SUCCESS();

  ASSERT_GE(call_syscall(SYS_close, write_fd), 0l);
  ASSERT_ERRNO_SUCCESS();

#ifdef SYS_open
  long dir_fd = call_syscall(SYS_open, TEST_DIR, O_DIRECTORY, 0);
#elif defined(SYS_openat)
  long dir_fd = call_syscall(SYS_openat, AT_FDCWD, TEST_DIR, O_DIRECTORY, 0);
#else
#error "open and openat syscalls not available."
#endif
  ASSERT_GT(dir_fd, 0l);
  ASSERT_ERRNO_SUCCESS();

  ASSERT_GE(
      call_syscall(SYS_linkat, dir_fd, TEST_FILE, dir_fd, TEST_FILE_LINK, 0),
      0l);
  ASSERT_ERRNO_SUCCESS();
#ifdef SYS_open
  long link_fd = call_syscall(SYS_open, TEST_FILE_LINK_PATH, O_PATH, 0);
#elif defined(SYS_openat)
  long link_fd =
      call_syscall(SYS_openat, AT_FDCWD, TEST_FILE_LINK_PATH, O_PATH, 0);
#else
#error "open and openat syscalls not available."
#endif
  ASSERT_GT(link_fd, 0l);
  ASSERT_ERRNO_SUCCESS();

#ifdef SYS_unlink
  ASSERT_GE(call_syscall(SYS_unlink, TEST_FILE_PATH), 0l);
#elif defined(SYS_unlinkat)
  ASSERT_GE(call_syscall(SYS_unlinkat, AT_FDCWD, TEST_FILE_PATH, 0), 0l);
#else
#error "unlink and unlinkat syscalls not available."
#endif
  ASSERT_ERRNO_SUCCESS();

#ifdef SYS_unlink
  ASSERT_GE(call_syscall(SYS_unlink, TEST_FILE_LINK_PATH), 0l);
#elif defined(SYS_unlinkat)
  ASSERT_GE(call_syscall(SYS_unlinkat, AT_FDCWD, TEST_FILE_LINK_PATH, 0), 0l);
#else
#error "unlink and unlinkat syscalls not available."
#endif
  ASSERT_ERRNO_SUCCESS();

  ASSERT_GE(call_syscall(SYS_close, dir_fd), 0l);
  ASSERT_ERRNO_SUCCESS();
}

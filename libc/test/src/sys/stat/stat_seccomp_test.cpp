//===-- Unittests for stat under a filter that refuses statx -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Seccomp filters written before statx existed, such as the one Firefox runs
/// its content processes under, refuse it. stat has to fall back to
/// newfstatat there instead of failing.
///
//===----------------------------------------------------------------------===//

#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_stat.h"
#include "src/__support/OSUtil/linux/syscall.h"
#include "src/stdlib/_Exit.h"
#include "src/sys/stat/stat.h"
#include "src/sys/wait/waitpid.h"
#include "src/unistd/fork.h"
#include "test/UnitTest/Test.h"

#include <linux/filter.h>
#include <linux/seccomp.h>
#include <sys/syscall.h>

#if defined(SYS_newfstatat) && defined(SYS_statx) && defined(SYS_seccomp)

namespace {

// Answers ENOSYS to statx and lets everything else through.
bool refuse_statx() {
  struct sock_filter program[] = {
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, 0),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, SYS_statx, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | 38),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
  };
  struct sock_fprog filter = {
      static_cast<unsigned short>(sizeof(program) / sizeof(program[0])),
      program};
  constexpr int SET_NO_NEW_PRIVS = 38;
  constexpr int NO_NEW_PRIVS_ON = 1;
  if (LIBC_NAMESPACE::syscall_impl<long>(SYS_prctl, SET_NO_NEW_PRIVS,
                                         NO_NEW_PRIVS_ON, 0, 0, 0) != 0)
    return false;
  return LIBC_NAMESPACE::syscall_impl<long>(SYS_seccomp,
                                            SECCOMP_SET_MODE_FILTER, 0,
                                            &filter) == 0;
}

} // anonymous namespace

TEST(LlvmLibcStatSeccompTest, FallsBackWhenStatxIsRefused) {
  // The filter cannot be taken off again, so it goes on a child.
  int pid = LIBC_NAMESPACE::fork();
  ASSERT_GE(pid, 0);
  if (pid == 0) {
    if (!refuse_statx())
      LIBC_NAMESPACE::_Exit(2);
    struct stat statbuf;
    // The filter is in force: statx itself is refused.
    if (LIBC_NAMESPACE::syscall_impl<long>(SYS_statx, -100, "/dev/null", 0, 0,
                                           &statbuf) != -38)
      LIBC_NAMESPACE::_Exit(3);
    if (LIBC_NAMESPACE::stat("/dev/null", &statbuf) != 0)
      LIBC_NAMESPACE::_Exit(4);
    // /dev/null is character device 1,3, which makedev encodes as 0x103.
    if (!S_ISCHR(statbuf.st_mode) || statbuf.st_rdev != 0x103)
      LIBC_NAMESPACE::_Exit(5);
    LIBC_NAMESPACE::_Exit(0);
  }
  int status = -1;
  ASSERT_EQ(LIBC_NAMESPACE::waitpid(pid, &status, 0), pid);
  ASSERT_EQ(status, 0);
}

#endif // SYS_newfstatat && SYS_statx && SYS_seccomp

//===-- Unittests for system under a filter that refuses clone3 ----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Kernels before Linux 5.3 and some seccomp policies answer ENOSYS for
/// clone3. system has to fall back to a plain fork there instead of failing.
///
//===----------------------------------------------------------------------===//

#include "hdr/sys_wait_macros.h"
#include "src/__support/OSUtil/linux/syscall.h"
#include "src/stdlib/_Exit.h"
#include "src/stdlib/system.h"
#include "src/sys/wait/waitpid.h"
#include "src/unistd/fork.h"
#include "test/UnitTest/Test.h"

#include <linux/filter.h>
#include <linux/seccomp.h>
#include <sys/syscall.h>

#if defined(SYS_clone3) && defined(SYS_seccomp)

namespace {

// Answers ENOSYS to clone3 and lets everything else through.
bool refuse_clone3() {
  struct sock_filter program[] = {
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, 0),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, SYS_clone3, 0, 1),
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

TEST(LlvmLibcSystemSeccompTest, FallsBackWhenClone3IsRefused) {
  // The filter cannot be taken off again, so it goes on a child.
  int pid = LIBC_NAMESPACE::fork();
  ASSERT_GE(pid, 0);
  if (pid == 0) {
    if (!refuse_clone3())
      LIBC_NAMESPACE::_Exit(2);
    // The filter is in force: clone3 itself is refused.
    if (LIBC_NAMESPACE::syscall_impl<long>(SYS_clone3, 0, 0) != -38)
      LIBC_NAMESPACE::_Exit(3);
    int status = LIBC_NAMESPACE::system("exit 42");
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 42)
      LIBC_NAMESPACE::_Exit(4);
    LIBC_NAMESPACE::_Exit(0);
  }
  int status = -1;
  ASSERT_EQ(LIBC_NAMESPACE::waitpid(pid, &status, 0), pid);
  ASSERT_EQ(status, 0);
}

#endif // SYS_clone3 && SYS_seccomp

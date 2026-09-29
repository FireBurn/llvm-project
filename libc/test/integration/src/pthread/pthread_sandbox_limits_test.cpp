//===-- Integration test for reading limits under a sandbox ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/struct_rlimit.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/compat/__llvm_libc_init_main_thread.h"
#include "src/pthread/pthread_attr_destroy.h"
#include "src/pthread/pthread_create.h"
#include "src/pthread/pthread_getattr_np.h"
#include "src/pthread/pthread_join.h"
#include "src/pthread/pthread_self.h"
#include "src/sys/resource/getrlimit.h"
#include "src/sys/resource/setrlimit.h"
#include "src/unistd/isatty.h"
#include "src/unistd/sysconf.h"
#include "test/IntegrationTest/test.h"

#include <linux/filter.h>
#include <linux/prctl.h>
#include <linux/seccomp.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <unistd.h>

// Firefox's and Chromium's sandboxes let a process read its own limits with
// getrlimit and kill it for prlimit64, which reaches other processes. Firefox's
// utility processes refuse getrlimit as well, once they are running, so the
// stack limit has to have been read at startup for threads to be made and
// described after that.

static void refuse(long number) {
  sock_filter filter[] = {
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, __builtin_offsetof(seccomp_data, nr)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, static_cast<unsigned>(number), 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
  };
  sock_fprog program = {sizeof(filter) / sizeof(filter[0]), filter};
  ASSERT_EQ(LIBC_NAMESPACE::syscall_impl<long>(SYS_prctl, PR_SET_NO_NEW_PRIVS,
                                               1, 0, 0, 0),
            0L);
  ASSERT_EQ(LIBC_NAMESPACE::syscall_impl<long>(
                SYS_seccomp, SECCOMP_SET_MODE_FILTER, 0, &program),
            0L);
}

// Firefox's plugin sandbox lets ioctl through only for the request that asks
// for a terminal's attributes, so isatty must ask that and nothing else.
static void refuse_ioctl_line_discipline() {
  sock_filter filter[] = {
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, __builtin_offsetof(seccomp_data, nr)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, SYS_ioctl, 0, 3),
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS,
               __builtin_offsetof(seccomp_data, args[1])),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, TIOCGETD, 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
  };
  sock_fprog program = {sizeof(filter) / sizeof(filter[0]), filter};
  ASSERT_EQ(LIBC_NAMESPACE::syscall_impl<long>(SYS_prctl, PR_SET_NO_NEW_PRIVS,
                                               1, 0, 0, 0),
            0L);
  ASSERT_EQ(LIBC_NAMESPACE::syscall_impl<long>(
                SYS_seccomp, SECCOMP_SET_MODE_FILTER, 0, &program),
            0L);
}

static void *idle(void *) { return nullptr; }

TEST_MAIN() {
  // The loader does this before the initialisers of what it loaded, and
  // with it the stack limit is read.
  LIBC_NAMESPACE::__llvm_libc_init_main_thread();

  refuse(SYS_prlimit64);
  refuse_ioctl_line_discipline();

  struct rlimit limit;
  ASSERT_EQ(LIBC_NAMESPACE::getrlimit(RLIMIT_CORE, &limit), 0);
  // Setting a limit to what it already is needs no privilege.
  ASSERT_EQ(LIBC_NAMESPACE::setrlimit(RLIMIT_CORE, &limit), 0);
  ASSERT_TRUE(LIBC_NAMESPACE::sysconf(_SC_OPEN_MAX) != 0);
  ASSERT_TRUE(LIBC_NAMESPACE::sysconf(_SC_CHILD_MAX) != 0);
  ASSERT_TRUE(LIBC_NAMESPACE::sysconf(_SC_ARG_MAX) != 0);

  // Whether a terminal or not, this must not be answered with a kill.
  LIBC_NAMESPACE::isatty(0);
  LIBC_NAMESPACE::isatty(1);

  // Now no limit may be read at all.
  refuse(SYS_getrlimit);

  pthread_attr_t attr;
  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_getattr_np(LIBC_NAMESPACE::pthread_self(), &attr),
      0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_destroy(&attr), 0);

  // Creating a thread needs the stack limit for its default size.
  pthread_t thread;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&thread, nullptr, idle, nullptr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(thread, nullptr), 0);
  return 0;
}

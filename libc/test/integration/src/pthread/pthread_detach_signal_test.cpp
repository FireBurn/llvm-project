//===-- Tests for detached threads exiting under signals ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/signal_macros.h"
#include "hdr/types/sigset_t.h"
#include "hdr/types/struct_sigaction.h"
#include "src/__support/CPP/atomic.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/pthread/pthread_attr_init.h"
#include "src/pthread/pthread_attr_setdetachstate.h"
#include "src/pthread/pthread_create.h"
#include "src/pthread/pthread_join.h"
#include "src/signal/kill.h"
#include "src/signal/pthread_sigmask.h"
#include "src/signal/sigaction.h"
#include "src/signal/sigaddset.h"
#include "src/signal/sigemptyset.h"
#include "src/unistd/getpid.h"
#include "test/IntegrationTest/test.h"

#include <linux/filter.h>
#include <linux/prctl.h>
#include <linux/seccomp.h>
#include <pthread.h>
#include <sys/syscall.h>

// A detached thread cannot give back the stack it runs on. Were it to, a
// signal arriving in its last moments would have nowhere to be delivered and
// the kernel would kill the process. It also makes no system call beyond exit
// on the way out, since sandboxes such as Chromium's allow little else.

static LIBC_NAMESPACE::cpp::Atomic<int> finished(0);
static LIBC_NAMESPACE::cpp::Atomic<int> stop(0);

static void on_signal(int) {}

static sigset_t usr1;

// Only these threads take the signal, so it lands on them as they exit.
static void *quick(void *) {
  LIBC_NAMESPACE::pthread_sigmask(SIG_UNBLOCK, &usr1, nullptr);
  finished.fetch_add(1);
  return nullptr;
}

static void *signaller(void *) {
  pid_t self = LIBC_NAMESPACE::getpid();
  while (stop.load() == 0)
    LIBC_NAMESPACE::kill(self, SIGUSR1);
  return nullptr;
}

TEST_MAIN() {
  struct sigaction sa = {};
  sa.sa_handler = on_signal;
  ASSERT_EQ(LIBC_NAMESPACE::sigaction(SIGUSR1, &sa, nullptr), 0);
  LIBC_NAMESPACE::sigemptyset(&usr1);
  LIBC_NAMESPACE::sigaddset(&usr1, SIGUSR1);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_sigmask(SIG_BLOCK, &usr1, nullptr), 0);

  pthread_t noisy;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&noisy, nullptr, signaller, nullptr),
            0);

  pthread_attr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_init(&attr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_setdetachstate(
                &attr, PTHREAD_CREATE_DETACHED),
            0);
  constexpr int THREADS = 2000;
  for (int i = 0; i < THREADS; ++i) {
    pthread_t thread;
    ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&thread, &attr, quick, nullptr),
              0);
  }
  while (finished.load() < THREADS)
    ;

  stop.store(1);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(noisy, nullptr), 0);

  // From here set_tid_address kills the process, as Chromium's renderer
  // sandbox does.
  sock_filter filter[] = {
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, __builtin_offsetof(seccomp_data, nr)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, SYS_set_tid_address, 0, 1),
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
  finished.store(0);
  for (int i = 0; i < 100; ++i) {
    pthread_t thread;
    ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&thread, &attr, quick, nullptr),
              0);
  }
  while (finished.load() < 100)
    ;
  // Creating one more collects the stacks of those that have gone.
  pthread_t last;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&last, nullptr, quick, nullptr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(last, nullptr), 0);
  return 0;
}

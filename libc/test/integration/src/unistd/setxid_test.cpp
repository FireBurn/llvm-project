//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration test for credential changes in a process with threads.
///
//===----------------------------------------------------------------------===//

#include "src/__support/CPP/atomic.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/pthread/pthread_create.h"
#include "src/pthread/pthread_join.h"
#include "src/signal/pthread_sigmask.h"
#include "src/signal/sigfillset.h"
#include "src/unistd/geteuid.h"
#include "src/unistd/setresgid.h"
#include "src/unistd/setresuid.h"
#include "src/unistd/setreuid.h"
#include "test/IntegrationTest/test.h"

#include <pthread.h>
#include <signal.h>
#include <sys/syscall.h>

constexpr int THREADS = 8;
constexpr uid_t NOBODY = 65534;

static LIBC_NAMESPACE::cpp::Atomic<int> started(0);
static LIBC_NAMESPACE::cpp::Atomic<int> stop(0);

static void *worker(void *arg) {
  // One thread blocks every signal it can, which must not keep it from
  // taking part.
  if (arg != nullptr) {
    sigset_t all;
    LIBC_NAMESPACE::sigfillset(&all);
    LIBC_NAMESPACE::pthread_sigmask(SIG_BLOCK, &all, nullptr);
  }
  started.fetch_add(1);
  while (!stop.load())
    ;
  // The credentials of this thread, which the kernel keeps per thread.
  return reinterpret_cast<void *>(
      LIBC_NAMESPACE::syscall_impl<long>(SYS_getuid));
}

TEST_MAIN() {
  pthread_t threads[THREADS];
  for (int i = 0; i < THREADS; ++i)
    ASSERT_EQ(LIBC_NAMESPACE::pthread_create(
                  &threads[i], nullptr, worker,
                  i == 0 ? reinterpret_cast<void *>(1) : nullptr),
              0);
  while (started.load() != THREADS)
    ;

  // Changing nothing still goes to every thread, and comes back.
  ASSERT_EQ(
      LIBC_NAMESPACE::setreuid(static_cast<uid_t>(-1), static_cast<uid_t>(-1)),
      0);

  bool privileged = LIBC_NAMESPACE::geteuid() == 0;
  if (privileged) {
    ASSERT_EQ(LIBC_NAMESPACE::setresgid(NOBODY, NOBODY, NOBODY), 0);
    ASSERT_EQ(LIBC_NAMESPACE::setresuid(NOBODY, NOBODY, NOBODY), 0);
  }

  stop.store(1);
  for (int i = 0; i < THREADS; ++i) {
    void *uid;
    ASSERT_EQ(LIBC_NAMESPACE::pthread_join(threads[i], &uid), 0);
    if (privileged)
      ASSERT_EQ(reinterpret_cast<long>(uid), static_cast<long>(NOBODY));
  }
  return 0;
}

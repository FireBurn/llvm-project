//===-- Tests for pthread_tryjoin_np --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_create.h"
#include "src/pthread/pthread_self.h"
#include "src/pthread/pthread_tryjoin_np.h"

#include "src/__support/CPP/atomic.h"
#include "test/IntegrationTest/test.h"

#include <errno.h>
#include <pthread.h>
#include <stdint.h>

static LIBC_NAMESPACE::cpp::Atomic<int> release(0);

static void *wait_to_be_released(void *) {
  while (release.load() == 0)
    ; // Spin until the test lets this thread finish.
  return reinterpret_cast<void *>(7);
}

// A running thread is reported busy, and is still there to be joined once it
// has finished.
static void busy_then_joinable_test() {
  pthread_t tid;
  release.store(0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&tid, nullptr, wait_to_be_released,
                                           nullptr),
            0);

  ASSERT_EQ(LIBC_NAMESPACE::pthread_tryjoin_np(tid, nullptr), EBUSY);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_tryjoin_np(tid, nullptr), EBUSY);

  release.store(1);

  void *retval = nullptr;
  int status;
  while ((status = LIBC_NAMESPACE::pthread_tryjoin_np(tid, &retval)) == EBUSY)
    ; // Spin until the thread has finished and can be joined.
  ASSERT_EQ(status, 0);
  ASSERT_EQ(reinterpret_cast<uintptr_t>(retval), uintptr_t(7));
}

static void *simple_func(void *) { return nullptr; }

static void null_retval_test() {
  pthread_t tid;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&tid, nullptr, simple_func, nullptr),
            0);
  int status;
  while ((status = LIBC_NAMESPACE::pthread_tryjoin_np(tid, nullptr)) == EBUSY)
    ;
  ASSERT_EQ(status, 0);
}

static void self_join_test() {
  ASSERT_EQ(LIBC_NAMESPACE::pthread_tryjoin_np(LIBC_NAMESPACE::pthread_self(),
                                               nullptr),
            EDEADLK);
}

TEST_MAIN() {
  errno = 0;
  busy_then_joinable_test();
  null_retval_test();
  self_join_test();
  return 0;
}

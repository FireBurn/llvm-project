//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration tests for robust mutexes and pthread_mutex_consistent.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/pthread/pthread_create.h"
#include "src/pthread/pthread_join.h"
#include "src/pthread/pthread_mutex_consistent.h"
#include "src/pthread/pthread_mutex_destroy.h"
#include "src/pthread/pthread_mutex_init.h"
#include "src/pthread/pthread_mutex_lock.h"
#include "src/pthread/pthread_mutex_unlock.h"
#include "src/pthread/pthread_mutexattr_destroy.h"
#include "src/pthread/pthread_mutexattr_init.h"
#include "src/pthread/pthread_mutexattr_setrobust.h"
#include "test/IntegrationTest/test.h"

#include <pthread.h>

static pthread_mutex_t mutex;

// Ends while holding the lock, which is the case a robust mutex exists for.
static void *die_holding_the_lock(void *) {
  LIBC_NAMESPACE::pthread_mutex_lock(&mutex);
  return nullptr;
}

static void init_robust() {
  pthread_mutexattr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_init(&attr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_setrobust(&attr,
                                                        PTHREAD_MUTEX_ROBUST),
            0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_init(&mutex, &attr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutexattr_destroy(&attr), 0);
}

static void run_owner_that_dies() {
  pthread_t tid;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&tid, nullptr, die_holding_the_lock,
                                           nullptr),
            0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(tid, nullptr), 0);
}

// The next to lock is told the owner is gone, and can make the state
// consistent and carry on.
static void owner_died_then_made_consistent() {
  init_robust();
  run_owner_that_dies();
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_lock(&mutex), EOWNERDEAD);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_consistent(&mutex), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_unlock(&mutex), 0);

  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_lock(&mutex), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_unlock(&mutex), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_destroy(&mutex), 0);
}

// Unlocking without having made it consistent leaves it unusable.
static void unlocked_without_consistent() {
  init_robust();
  run_owner_that_dies();
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_lock(&mutex), EOWNERDEAD);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_unlock(&mutex), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_lock(&mutex), ENOTRECOVERABLE);
}

// A mutex that is not waiting to be made consistent refuses.
static void consistent_on_a_healthy_mutex() {
  init_robust();
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_consistent(&mutex), EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_destroy(&mutex), 0);
}

TEST_MAIN() {
  owner_died_then_made_consistent();
  unlocked_without_consistent();
  consistent_on_a_healthy_mutex();
  return 0;
}

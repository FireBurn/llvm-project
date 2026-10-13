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
#include "hdr/stdint_proxy.h"
#include "src/__support/OSUtil/syscall.h"
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
#include <stddef.h>
#include <sys/syscall.h>

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

// What the kernel is given, as it lays it out.
struct KernelRobustHead {
  uintptr_t next;
  long futex_offset;
  uintptr_t pending;
};

static bool layout_as_glibc = false;

// A program which inspects the list, as Steam's overlay does, expects the
// kernel to be told glibc's offset from a node to the futex word, and the node
// to be the one a mutex keeps thirty two bytes after its futex word.
static void *inspect_list(void *) {
  KernelRobustHead *head = nullptr;
  size_t size = 0;
  if (LIBC_NAMESPACE::syscall_impl<long>(SYS_get_robust_list, 0, &head,
                                         &size) != 0 ||
      head == nullptr || size != sizeof(KernelRobustHead))
    return nullptr;
  if (head->futex_offset != -32)
    return nullptr;
  // Nothing held, so the list leads back to its own head.
  if ((head->next & ~uintptr_t(1)) != reinterpret_cast<uintptr_t>(head))
    return nullptr;

  LIBC_NAMESPACE::pthread_mutex_lock(&mutex);
  const uintptr_t node = head->next & ~uintptr_t(1);
  const bool on_the_list =
      node == reinterpret_cast<uintptr_t>(&mutex) + 32;
  LIBC_NAMESPACE::pthread_mutex_unlock(&mutex);
  const bool back_to_empty =
      (head->next & ~uintptr_t(1)) == reinterpret_cast<uintptr_t>(head);
  layout_as_glibc = on_the_list && back_to_empty;
  return nullptr;
}

static void list_is_laid_out_as_glibc() {
  init_robust();
  pthread_t tid;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&tid, nullptr, inspect_list,
                                           nullptr),
            0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(tid, nullptr), 0);
  ASSERT_TRUE(layout_as_glibc);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_destroy(&mutex), 0);
}

TEST_MAIN() {
  list_is_laid_out_as_glibc();
  owner_died_then_made_consistent();
  unlocked_without_consistent();
  consistent_on_a_healthy_mutex();
  return 0;
}

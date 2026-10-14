//===-- Unittests for the stack guard of a new thread ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Code built with a stack protector can be entered on one thread and left on
/// another when a fiber library moves a stack, so every thread has to carry
/// the guard of the process, as with glibc.
///
//===----------------------------------------------------------------------===//

#include "hdr/stdint_proxy.h"
#include "src/pthread/pthread_create.h"
#include "src/pthread/pthread_join.h"
#include "test/IntegrationTest/test.h"

#include <pthread.h>

#ifdef __x86_64__

// The guard sits at a fixed place in the thread control block, which is what
// the compiler reads it from.
static uintptr_t guard() {
  uintptr_t value;
  asm volatile("mov %%fs:0x28, %0" : "=r"(value));
  return value;
}

static void *read_guard(void *) {
  return reinterpret_cast<void *>(guard());
}

TEST_MAIN() {
  const uintptr_t mine = guard();
  ASSERT_TRUE(mine != 0);

  // Each thread, and one started from another thread, has the same guard.
  for (int i = 0; i < 4; ++i) {
    pthread_t thread;
    ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&thread, nullptr, read_guard,
                                             nullptr),
              0);
    void *result = nullptr;
    ASSERT_EQ(LIBC_NAMESPACE::pthread_join(thread, &result), 0);
    ASSERT_EQ(reinterpret_cast<uintptr_t>(result), mine);
  }
  return 0;
}

#else

TEST_MAIN() { return 0; }

#endif // __x86_64__

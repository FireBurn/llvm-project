//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration test for __libc_single_threaded
///
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_create.h"
#include "src/pthread/pthread_join.h"
#include "src/sys/single_threaded/__libc_single_threaded.h"
#include "test/IntegrationTest/test.h"

#include <pthread.h>

static void *thread_func(void *) {
  ASSERT_EQ(LIBC_NAMESPACE::__libc_single_threaded, 0);
  return nullptr;
}

TEST_MAIN() {
  ASSERT_EQ(LIBC_NAMESPACE::__libc_single_threaded, 1);

  pthread_t thread;
  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_create(&thread, nullptr, thread_func, nullptr),
      0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(thread, nullptr), 0);

  // Once a thread has been made the process is not single threaded again.
  ASSERT_EQ(LIBC_NAMESPACE::__libc_single_threaded, 0);
  return 0;
}

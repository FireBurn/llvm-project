//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration tests for pthread_setattr_default_np and
/// pthread_getattr_default_np.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/pthread/pthread_attr_destroy.h"
#include "src/pthread/pthread_attr_getstacksize.h"
#include "src/pthread/pthread_attr_init.h"
#include "src/pthread/pthread_attr_setstacksize.h"
#include "src/pthread/pthread_create.h"
#include "src/pthread/pthread_getattr_default_np.h"
#include "src/pthread/pthread_getattr_np.h"
#include "src/pthread/pthread_join.h"
#include "src/pthread/pthread_self.h"
#include "src/pthread/pthread_setattr_default_np.h"
#include "test/IntegrationTest/test.h"

#include <pthread.h>

constexpr size_t STACK = 512 * 1024;

static void *report_stack_size(void *out) {
  pthread_attr_t attr;
  if (LIBC_NAMESPACE::pthread_getattr_np(LIBC_NAMESPACE::pthread_self(),
                                         &attr) != 0)
    return reinterpret_cast<void *>(1);
  size_t size = 0;
  LIBC_NAMESPACE::pthread_attr_getstacksize(&attr, &size);
  LIBC_NAMESPACE::pthread_attr_destroy(&attr);
  *static_cast<size_t *>(out) = size;
  return nullptr;
}

TEST_MAIN() {
  pthread_attr_t original;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_getattr_default_np(&original), 0);

  pthread_attr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_init(&attr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_setstacksize(&attr, STACK), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_setattr_default_np(&attr), 0);

  // What was set is what comes back.
  pthread_attr_t fetched;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_getattr_default_np(&fetched), 0);
  size_t size = 0;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_getstacksize(&fetched, &size), 0);
  ASSERT_EQ(size, STACK);

  // A thread made without attributes takes the default.
  size_t seen = 0;
  pthread_t tid;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&tid, nullptr, report_stack_size,
                                           &seen),
            0);
  void *result = reinterpret_cast<void *>(2);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(tid, &result), 0);
  ASSERT_TRUE(result == nullptr);
  ASSERT_EQ(seen, STACK);

  // A stack too small to run on is refused.
  pthread_attr_t tiny;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_init(&tiny), 0);
  tiny.__stacksize = 16;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_setattr_default_np(&tiny), EINVAL);

  ASSERT_EQ(LIBC_NAMESPACE::pthread_setattr_default_np(&original), 0);
  LIBC_NAMESPACE::pthread_attr_destroy(&attr);
  LIBC_NAMESPACE::pthread_attr_destroy(&fetched);
  return 0;
}

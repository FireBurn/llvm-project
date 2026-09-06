//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests for the stack a thread is given when none is asked for.
///
//===----------------------------------------------------------------------===//

#include "hdr/stdint_proxy.h"
#include "hdr/sys_resource_macros.h"
#include "hdr/types/struct_rlimit.h"
#include "src/pthread/pthread_attr_getstacksize.h"
#include "src/pthread/pthread_attr_init.h"
#include "src/pthread/pthread_create.h"
#include "src/pthread/pthread_join.h"
#include "src/sys/resource/getrlimit.h"
#include "test/IntegrationTest/test.h"

#include <pthread.h>

constexpr size_t FRAME = 4096;
constexpr size_t DEPTH = 384; // 1.5 MiB of stack in all.

// Each level keeps a frame of its own live, so the depth is really used.
static size_t recurse(size_t depth) {
  volatile char frame[FRAME];
  frame[0] = static_cast<char>(depth);
  frame[FRAME - 1] = static_cast<char>(depth);
  if (depth == 0)
    return frame[0];
  return recurse(depth - 1) + frame[FRAME - 1];
}

static void *deep(void *) {
  recurse(DEPTH);
  return reinterpret_cast<void *>(uintptr_t(1));
}

TEST_MAIN() {
  struct rlimit limit;
  ASSERT_EQ(LIBC_NAMESPACE::getrlimit(RLIMIT_STACK, &limit), 0);

  // A fresh attribute reports the limit set on the process, unless there is
  // none, when it is a fixed size which is still a usable one.
  pthread_attr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_init(&attr), 0);
  size_t size = 0;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_getstacksize(&attr, &size), 0);
  if (limit.rlim_cur != RLIM_INFINITY && limit.rlim_cur >= (16u << 10))
    ASSERT_EQ(size, static_cast<size_t>(limit.rlim_cur));
  ASSERT_TRUE(size >= (16u << 10));

  // A thread started with no attribute can use a good part of that stack.
  // The old default of sixty four kilobytes could not.
  if (size > DEPTH * FRAME + (256u << 10)) {
    pthread_t th;
    ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&th, nullptr, deep, nullptr), 0);
    void *retval = nullptr;
    ASSERT_EQ(LIBC_NAMESPACE::pthread_join(th, &retval), 0);
    ASSERT_EQ(retval, reinterpret_cast<void *>(uintptr_t(1)));
  }
  return 0;
}

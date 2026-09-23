//===-- Implementation of the pthread_attr_init ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "pthread_attr_init.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/thread.h" // For thread::DEFAULT_*

#include <pthread.h>

namespace LIBC_NAMESPACE_DECL {

// Programs keep these on their stacks, sized by the header they were built
// against, so growing it overruns every one of them built before.
static_assert(sizeof(pthread_attr_t) ==
                  2 * sizeof(int) + sizeof(void *) + 2 * sizeof(size_t),
              "pthread_attr_t must keep its size");

LLVM_LIBC_FUNCTION(int, pthread_attr_init, (pthread_attr_t * attr)) {
  *attr = pthread_attr_t{
      PTHREAD_CREATE_JOINABLE,     // Not detached
      PTHREAD_INHERIT_SCHED,       // Scheduling comes from the creating thread
      SCHED_OTHER,                 // which leaves these two unused until a
      0,                           // caller asks for the explicit form.
      nullptr,                     // Let the thread manage its stack
      Thread::default_stacksize(), // stack size, as the process is allowed.
      Thread::DEFAULT_GUARDSIZE,   // Default page size for the guard size.
  };
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

//===-- Implementation of pthread_getcpuclockid ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "pthread_getcpuclockid.h"

#include "hdr/errno_macros.h"
#include "hdr/types/clockid_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/thread.h"

#include <pthread.h>

namespace LIBC_NAMESPACE_DECL {

namespace {

// How the kernel reads a clock id which names a thread rather than a clock
// of its own: the identifier is folded into the upper bits and the low
// three say what kind of clock it is. See MAKE_THREAD_CPUCLOCK in the
// kernel's posix-timers.h.
constexpr clockid_t CPUCLOCK_SCHED = 2;
constexpr clockid_t CPUCLOCK_PERTHREAD_MASK = 4;

constexpr clockid_t thread_cpuclock(int tid) {
  return (~static_cast<clockid_t>(tid) << 3) |
         (CPUCLOCK_SCHED | CPUCLOCK_PERTHREAD_MASK);
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(int, pthread_getcpuclockid,
                   (pthread_t thread, clockid_t *clockid)) {
  if (clockid == nullptr)
    return EINVAL;

  auto *th = reinterpret_cast<Thread *>(&thread);
  if (th->attrib == nullptr)
    return ESRCH;

  *clockid = thread_cpuclock(th->attrib->tid);
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

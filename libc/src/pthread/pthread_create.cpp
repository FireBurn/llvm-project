//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of the pthread_create function.
///
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_create.h"

#include "hdr/errno_macros.h"
#include "hdr/pthread_macros.h"
#include "hdr/types/pthread_attr_t.h"
#include "hdr/types/pthread_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/thread.h"
#include "src/pthread/pthread_attr.h"

namespace LIBC_NAMESPACE_DECL {

static_assert(sizeof(pthread_t) == sizeof(LIBC_NAMESPACE::Thread),
              "Mismatch between pthread_t and internal Thread.");

LLVM_LIBC_FUNCTION(int, pthread_create,
                   (pthread_t *__restrict th,
                    const pthread_attr_t *__restrict attr,
                    __pthread_start_t func, void *arg)) {
  // Without attributes a thread gets the stack the process is allowed, which
  // Thread::run works out when asked for no particular size.
  bool defaulted = attr == nullptr;
  if (defaulted)
    attr = &DEFAULT_PTHREAD_ATTR;

  void *stack = attr->__stack;
  size_t stacksize = defaulted ? 0 : attr->__stacksize;
  size_t guardsize = attr->__guardsize;
  int detachstate = attr->__detachstate;
  int inheritsched = attr->__inheritsched;
  int schedpolicy = attr->__schedpolicy;
  struct sched_param schedparam = attr->__schedparam;

  if (stacksize && stacksize < PTHREAD_STACK_MIN)
    return EINVAL;

  if (guardsize % EXEC_PAGESIZE != 0)
    return EINVAL;

  if (detachstate != PTHREAD_CREATE_DETACHED &&
      detachstate != PTHREAD_CREATE_JOINABLE)
    return EINVAL;

  // Thread::run will check validity of the `stack` argument (stack alignment is
  // universal, not sure a pthread requirement).

  auto *thread = reinterpret_cast<LIBC_NAMESPACE::Thread *>(th);
  int result = thread->run(func, arg, stack, stacksize, guardsize,
                           detachstate == PTHREAD_CREATE_DETACHED);
  if (result != 0 && result != EPERM && result != EINVAL)
    return EAGAIN;
  if (result != 0)
    return result;

  // A thread inherits its creator's scheduling unless the attributes say to
  // use their own. Asking for a policy the process may not have is the
  // kernel's to refuse, and it refuses here rather than when it was set.
  if (inheritsched == PTHREAD_EXPLICIT_SCHED) {
    int sched_result =
        thread->setschedparam(SchedParameters{schedpolicy, schedparam});
    if (sched_result != 0)
      return sched_result;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

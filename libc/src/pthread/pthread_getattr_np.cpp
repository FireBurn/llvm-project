//===-- Implementation of pthread_getattr_np ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_getattr_np.h"

#include "hdr/errno_macros.h"
#include "hdr/sys_resource_macros.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/prlimit.h"
#include "src/__support/common.h"
#include "src/__support/libc_assert.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/__support/threads/thread.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_getattr_np,
                   (pthread_t th, pthread_attr_t *attr)) {
  LIBC_CRASH_ON_NULLPTR(attr);

  auto *thread = reinterpret_cast<Thread *>(&th);
  if (thread->attrib == nullptr)
    return ESRCH;

  attr->__detachstate = static_cast<int>(thread->attrib->detach_state);
  attr->__guardsize = thread->attrib->guardsize;
  attr->__stack = thread->attrib->stack;
  attr->__stacksize = thread->attrib->stacksize;

  // The first thread's stack has no size of its own: it is given a page at a
  // time as it is used, up to whatever the limit on it says. Callers ask for
  // this to know where the stack ends, so the limit is what to report, and
  // the low address follows from it.
  if (attr->__stacksize == PTHREAD_STACK_DYNAMIC_NP) {
    rlimit limit;
    auto result = linux_syscalls::prlimit(0, RLIMIT_STACK, nullptr, &limit);
    if (!result.has_value())
      return static_cast<int>(result.error());

    size_t room = limit.rlim_cur;
    // Without a limit there is no low address to state, so a size is chosen
    // that describes the stack rather than the whole of memory.
    if (room == RLIM_INFINITY)
      room = size_t(8) << 20;
    attr->__stacksize = room;
    attr->__stack = reinterpret_cast<void *>(
        reinterpret_cast<uintptr_t>(thread->attrib->stack) - room);
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of clone.
///
//===----------------------------------------------------------------------===//

#include "src/sched/clone.h"
#include "hdr/errno_macros.h"
#include "hdr/sched_macros.h"
#include "hdr/types/pid_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/clone.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/properties/architectures.h"
#include "src/__support/threads/identifier.h"
#include <stdarg.h>
#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, clone,
                   (int (*func)(void *), void *stack, int flags, void *arg,
                    ...)) {
  // With neither a function nor a stack the child carries on from the call, as
  // it does after fork. bionic does the same, and debuggerd's crash handler
  // relies on it.
  const bool continues_here = func == nullptr && stack == nullptr;
  if (!continues_here && (func == nullptr || stack == nullptr)) {
    libc_errno = EINVAL;
    return -1;
  }
  // The child would run on the stack of the parent while sharing it.
  if (continues_here && (flags & CLONE_VM)) {
    libc_errno = EINVAL;
    return -1;
  }

  pid_t *parent_tid = nullptr;
  void *tls = nullptr;
  pid_t *child_tid = nullptr;

  if (flags & (CLONE_PARENT_SETTID | CLONE_SETTLS | CLONE_CHILD_SETTID |
               CLONE_CHILD_CLEARTID | CLONE_PIDFD)) {
    va_list args;
    va_start(args, arg);
    parent_tid = va_arg(args, pid_t *);
    if (flags & (CLONE_SETTLS | CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID))
      tls = va_arg(args, void *);
    if (flags & (CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID))
      child_tid = va_arg(args, pid_t *);
    va_end(args);
  }

  if (!continues_here) {
    uintptr_t stack_as_integer = reinterpret_cast<uintptr_t>(stack);
    stack_as_integer &= ~(linux_syscalls::CLONE_STACK_ALIGNMENT - 1);
    stack = reinterpret_cast<void *>(stack_as_integer);
  }

  // In some situations, we need to invalidate parent's tid cache. We cannot do
  // this in the child because a signal handler may observe the wrong tid before
  // we get a chance to set it.  We need to clear the tid if:
  // - the child is sharing the TLS block with us (!clone_tls); and
  // - the child is NOT sharing the address space with us (!clone_vm) or the
  //   parent-child execution is serialized (clone_vfork).
  // We cannot safely do this in the clone_vm && !clone_vfork case, as the
  // parent and child would race to overwrite each other's values, so we leave
  // this case unsupported.
  bool clone_settls = flags & CLONE_SETTLS;
  bool clone_vm = flags & CLONE_VM;
  bool clone_vfork = flags & CLONE_VFORK;
  bool should_clear_tid = !clone_settls && (!clone_vm || clone_vfork);

  pid_t tid_of_parent;
  if (should_clear_tid) {
    tid_of_parent = internal::gettid();
    internal::force_set_tid(0);
  }

  if (continues_here) {
    // The kernel orders the last two arguments by architecture.
#if defined(LIBC_TARGET_ARCH_IS_X86_64)
    long result = syscall_impl<long>(SYS_clone, flags, nullptr, parent_tid,
                                     child_tid, tls);
#else
    long result = syscall_impl<long>(SYS_clone, flags, nullptr, parent_tid, tls,
                                     child_tid);
#endif
    // The child leaves the cached id cleared for the next gettid to fill.
    if (result != 0 && should_clear_tid)
      internal::force_set_tid(tid_of_parent);
    if (result < 0) {
      libc_errno = static_cast<int>(-result);
      return -1;
    }
    return static_cast<int>(result);
  }

  auto ret = linux_syscalls::clone(func, stack, flags, arg, parent_tid, tls,
                                   child_tid);

  if (should_clear_tid)
    internal::force_set_tid(tid_of_parent);

  if (!ret.has_value()) {
    libc_errno = ret.error();
    return -1;
  }
  return ret.value();
}

} // namespace LIBC_NAMESPACE_DECL

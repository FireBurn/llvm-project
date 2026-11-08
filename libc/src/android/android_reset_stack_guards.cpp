//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
///
/// \file
/// Implementation of android_reset_stack_guards.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_reset_stack_guards.h"
#include "hdr/stdint_proxy.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getrandom.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/properties/architectures.h"
#include "src/__support/threads/tcb.h"

extern "C" uintptr_t __stack_chk_guard;

namespace LIBC_NAMESPACE_DECL {

// The zygote calls this in a child it has just forked, so that the child does
// not share its canary with the zygote and every sibling. A frame that is
// live when the canary changes fails its check on return, so the caller must
// not return through one that was built with the stack protector. This
// function's own frame is one of them, so it carries no check.
//
// The canary of the main thread is in the thread control block on x86_64 and
// in the global elsewhere. The global is written everywhere since it is what a
// thread that is created later copies.
[[gnu::no_stack_protector]] LLVM_LIBC_FUNCTION(void, android_reset_stack_guards,
                                               ()) {
  uintptr_t guard = 0;
  // The kernel returns all of a request this small, and a failure leaves the
  // old canary in place rather than weakening it.
  auto filled = linux_syscalls::getrandom(&guard, sizeof(guard), 0);
  if (!filled.has_value() || filled.value() != sizeof(guard))
    return;
  __stack_chk_guard = guard;
#ifdef LIBC_TARGET_ARCH_IS_X86_64
  get_tcb()->stack_guard = guard;
#endif
}

} // namespace LIBC_NAMESPACE_DECL

//===-- Implementation of __llvm_libc_init_main_thread --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/compat/__llvm_libc_init_main_thread.h"

#include "hdr/pthread_macros.h"
#include "hdr/sys_auxv_macros.h"
#include "src/__support/OSUtil/linux/auxv.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/tcb.h"
#include "src/__support/threads/thread.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

static ThreadAttributes main_thread_attrib;

// The startup code describes the thread it was entered on before anything
// asks about it, and a program that did not start through it has nobody to
// do that. Until it is done the attributes in the TCB are null, and
// pthread_self, pthread_equal and everything reached through pthread_once fault
// on it. The loader calls this too, since the initialisers it runs come before
// the startup code.
//
// Calling this more than once is harmless: the second call sees the work
// already done and leaves it alone.
LLVM_LIBC_FUNCTION(int, __llvm_libc_init_main_thread, (void)) {
  if (get_current_thread_attrib() != nullptr)
    return 0;

  long tid = syscall_impl<long>(SYS_gettid);
  if (tid <= 0)
    return -1;

  main_thread_attrib.tid = static_cast<int>(tid);

  // Where the first thread's stack ends. The kernel puts the executable's
  // path at the top of it and names it in the auxiliary vector, so rounding
  // that up to a page gives the top. pthread_getattr_np subtracts the limit
  // from this to report the low address, so leaving it zero would hand
  // callers a wrapped-around one.
  uintptr_t page_size = auxv::get(AT_PAGESZ).value_or(EXEC_PAGESIZE);
  if (page_size == 0)
    page_size = EXEC_PAGESIZE;
  uintptr_t stack_top = 0;
  if (auto execfn = auxv::get(AT_EXECFN))
    stack_top = *execfn;
  else
    // Nothing named it, so this frame is the best guide to where the stack
    // is. It is below the top rather than at it, which understates the
    // stack a little and never overstates it.
    stack_top = reinterpret_cast<uintptr_t>(&page_size);
  main_thread_attrib.stack =
      reinterpret_cast<void *>((stack_top + page_size - 1) & ~(page_size - 1));
  // The stack grows as the process runs rather than being a block this
  // library placed, so there is no fixed size to report.
  main_thread_attrib.stacksize = PTHREAD_STACK_DYNAMIC_NP;
  get_tcb()->attrib = &main_thread_attrib;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

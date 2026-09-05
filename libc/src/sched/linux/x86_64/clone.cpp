//===-- x86_64 implementation of clone ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sched/clone.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// Starts a thread of execution running |fn| on |stack|. The syscall itself
// comes back in both the parent and the child at the same place, with the
// child on a stack which holds none of what it needs; this hands the child
// straight to |fn| instead, and ends it with what |fn| gave back.
//
// |stack| points at the top of the block, since the stack grows down. The
// three arguments after |arg| are the ones only some of the flags call for:
// where to write the new thread id in the parent, the thread pointer, and
// where to write it in the child.
//
// It is naked because the arguments have to be moved into the registers the
// syscall wants, and because the child comes back on a stack of its own
// where a compiler's prologue would have nothing to unwind.
__attribute__((naked)) LLVM_LIBC_FUNCTION(int, clone,
                                          (int (*fn)(void *), void *stack,
                                           int flags, void *arg, ...)) {
  asm(R"(
      # On the way in: rdi = fn, rsi = stack, rdx = flags, rcx = arg,
      # r8 = parent_tid, r9 = tls, 8(%%rsp) = child_tid.
      #
      # What the syscall wants: rdi = flags, rsi = stack, rdx = parent_tid,
      # r10 = child_tid, r8 = tls.

      movq %%rdi, %%r11          # Keep fn somewhere the shuffle will not
      movq %%rdx, %%rdi          # touch, then move the flags into place.
      movq %%r8, %%rdx           # parent_tid
      movq %%r9, %%r8            # tls
      movq 8(%%rsp), %%r10       # child_tid
      movq %%r11, %%r9           # fn, for the child to call

      # The child's stack has to be sixteen byte aligned, and the argument
      # goes on it because the syscall hands the child no registers of its
      # own.
      andq $-16, %%rsi
      subq $8, %%rsi
      movq %%rcx, (%%rsi)

      movq %[clone], %%rax
      syscall

      testq %%rax, %%rax
      jnz 1f

      # The child. Ending the frame chain here stops a backtrace from
      # walking off into whatever the stack happened to hold.
      xorl %%ebp, %%ebp
      popq %%rdi                 # arg
      callq *%%r9                # fn(arg)

      movl %%eax, %%edi          # Its answer is the thread's exit status.
      movq %[exit], %%rax
      syscall
      hlt                        # exit does not come back.

      1:
      retq
      )" ::[clone] "i"(SYS_clone),
      [exit] "i"(SYS_exit)
      : "memory");
}

} // namespace LIBC_NAMESPACE_DECL

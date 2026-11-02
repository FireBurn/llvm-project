//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of setcontext for aarch64
///
//===----------------------------------------------------------------------===//

#include "src/ucontext/setcontext.h"
#include "include/llvm-libc-types/ucontext_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include "hdr/types/size_t.h"
#include "include/llvm-libc-macros/signal-macros.h"
#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

#define LIBC_FPSIMD_MAGIC 0x46508001

#define LIBC_UC_OFFSET(field) __builtin_offsetof(ucontext_t, uc_mcontext.field)

__attribute__((naked)) LLVM_LIBC_FUNCTION(int, setcontext,
                                          (const ucontext_t *ucp)) noexcept {
  asm(R"(
      // x0 is ucp. Keep it in x9 across the system call.
      mov x9, x0

      // rt_sigprocmask(SIG_SETMASK, &ucp->uc_sigmask, NULL, sigsetsize)
      add x1, x9, #%c[sigmask]
      mov x2, xzr
      mov x0, #%c[sig_setmask]
      mov x3, #%c[sigset_size]
      mov x8, #%c[syscall_num]
      svc #0

      // The floating point state, if the context carries the record.
      add x10, x9, #%c[fpsimd]
      ldr w2, [x10]
      mov w3, #(%c[magic] & 0xffff)
      movk w3, #(%c[magic] >> 16), lsl #16
      cmp w2, w3
      b.ne 1f
      ldp w2, w3, [x10, #8]
      msr fpsr, x2
      msr fpcr, x3
      ldp q8, q9, [x10, #(16 + 8 * 16)]
      ldp q10, q11, [x10, #(16 + 10 * 16)]
      ldp q12, q13, [x10, #(16 + 12 * 16)]
      ldp q14, q15, [x10, #(16 + 14 * 16)]
1:
      // Argument registers, so that a context made by makecontext starts its
      // function with its arguments, and the registers the ABI preserves.
      ldp x0, x1, [x9, #%c[x0]]
      ldp x2, x3, [x9, #%c[x2]]
      ldp x4, x5, [x9, #%c[x4]]
      ldp x6, x7, [x9, #%c[x6]]
      ldp x18, x19, [x9, #%c[x18]]
      ldp x20, x21, [x9, #%c[x20]]
      ldp x22, x23, [x9, #%c[x22]]
      ldp x24, x25, [x9, #%c[x24]]
      ldp x26, x27, [x9, #%c[x26]]
      ldp x28, x29, [x9, #%c[x28]]
      ldr x30, [x9, #%c[x30]]
      ldr x10, [x9, #%c[sp]]
      mov sp, x10
      ldr x10, [x9, #%c[pc]]
      br x10
      )" ::[sigset_size] "i"(__KERNEL_SIGSET_BYTES),
      [syscall_num] "i"(SYS_rt_sigprocmask), [sig_setmask] "i"(SIG_SETMASK),
      [magic] "i"(LIBC_FPSIMD_MAGIC), [x0] "i"(LIBC_UC_OFFSET(regs[0])),
      [x2] "i"(LIBC_UC_OFFSET(regs[2])), [x4] "i"(LIBC_UC_OFFSET(regs[4])),
      [x6] "i"(LIBC_UC_OFFSET(regs[6])), [x18] "i"(LIBC_UC_OFFSET(regs[18])),
      [x20] "i"(LIBC_UC_OFFSET(regs[20])), [x22] "i"(LIBC_UC_OFFSET(regs[22])),
      [x24] "i"(LIBC_UC_OFFSET(regs[24])), [x26] "i"(LIBC_UC_OFFSET(regs[26])),
      [x28] "i"(LIBC_UC_OFFSET(regs[28])), [x30] "i"(LIBC_UC_OFFSET(regs[30])),
      [sp] "i"(LIBC_UC_OFFSET(sp)), [pc] "i"(LIBC_UC_OFFSET(pc)),
      [fpsimd] "i"(LIBC_UC_OFFSET(__reserved)),
      [sigmask] "i"(__builtin_offsetof(ucontext_t, uc_sigmask))
      : "memory");
}

} // namespace LIBC_NAMESPACE_DECL

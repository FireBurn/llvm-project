//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of getcontext for aarch64
///
//===----------------------------------------------------------------------===//

#include "src/ucontext/getcontext.h"
#include "include/llvm-libc-types/ucontext_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include "hdr/types/size_t.h"
#include "include/llvm-libc-macros/signal-macros.h"
#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// The kernel's record for the floating point registers, the first in
// uc_mcontext.__reserved: magic, size, fpsr, fpcr and the 32 vector
// registers. A zero record follows it.
#define LIBC_FPSIMD_MAGIC 0x46508001
#define LIBC_FPSIMD_SIZE 0x210

#define LIBC_UC_OFFSET(field) __builtin_offsetof(ucontext_t, uc_mcontext.field)

// Naked so that the registers are the ones at the call, without a prologue.
// Only x0-x3, x8 and x9 are used, which the call may clobber anyway.
__attribute__((naked)) LLVM_LIBC_FUNCTION(int, getcontext,
                                          (ucontext_t * ucp)) noexcept {
  asm(R"(
      // x0 is ucp. Save the registers the ABI has a function preserve, and
      // the frame pointer and link register with them.
      stp x18, x19, [x0, #%c[x18]]
      stp x20, x21, [x0, #%c[x20]]
      stp x22, x23, [x0, #%c[x22]]
      stp x24, x25, [x0, #%c[x24]]
      stp x26, x27, [x0, #%c[x26]]
      stp x28, x29, [x0, #%c[x28]]
      str x30, [x0, #%c[x30]]

      // Resuming goes back to the caller, with the caller's stack pointer.
      mov x9, sp
      str x9, [x0, #%c[sp]]
      str x30, [x0, #%c[pc]]
      // getcontext returns 0 when it is resumed by setcontext.
      str xzr, [x0, #%c[x0]]
      str xzr, [x0, #%c[pstate]]

      // The floating point control state and the registers the ABI has
      // preserved, v8 to v15, in the kernel's record.
      add x1, x0, #%c[fpsimd]
      mov w2, #(%c[magic] & 0xffff)
      movk w2, #(%c[magic] >> 16), lsl #16
      mov w3, #%c[size]
      stp w2, w3, [x1]
      mrs x2, fpsr
      mrs x3, fpcr
      stp w2, w3, [x1, #8]
      stp q8, q9, [x1, #(16 + 8 * 16)]
      stp q10, q11, [x1, #(16 + 10 * 16)]
      stp q12, q13, [x1, #(16 + 12 * 16)]
      stp q14, q15, [x1, #(16 + 14 * 16)]
      str xzr, [x1, #%c[size]]

      // rt_sigprocmask(SIG_BLOCK, NULL, &ucp->uc_sigmask, sigsetsize)
      add x2, x0, #%c[sigmask]
      mov x1, xzr
      mov x0, #%c[sig_block]
      mov x3, #%c[sigset_size]
      mov x8, #%c[syscall_num]
      svc #0

      mov x0, xzr
      ret
      )" ::[sigset_size] "i"(__KERNEL_SIGSET_BYTES),
      [syscall_num] "i"(SYS_rt_sigprocmask), [sig_block] "i"(SIG_BLOCK),
      [magic] "i"(LIBC_FPSIMD_MAGIC), [size] "i"(LIBC_FPSIMD_SIZE),
      [x0] "i"(LIBC_UC_OFFSET(regs[0])), [x18] "i"(LIBC_UC_OFFSET(regs[18])),
      [x20] "i"(LIBC_UC_OFFSET(regs[20])), [x22] "i"(LIBC_UC_OFFSET(regs[22])),
      [x24] "i"(LIBC_UC_OFFSET(regs[24])), [x26] "i"(LIBC_UC_OFFSET(regs[26])),
      [x28] "i"(LIBC_UC_OFFSET(regs[28])), [x30] "i"(LIBC_UC_OFFSET(regs[30])),
      [sp] "i"(LIBC_UC_OFFSET(sp)), [pc] "i"(LIBC_UC_OFFSET(pc)),
      [pstate] "i"(LIBC_UC_OFFSET(pstate)),
      [fpsimd] "i"(LIBC_UC_OFFSET(__reserved)),
      [sigmask] "i"(__builtin_offsetof(ucontext_t, uc_sigmask))
      : "memory", "x1", "x2", "x3", "x8", "x9");
}

} // namespace LIBC_NAMESPACE_DECL

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

#include "src/ucontext/setcontext.h"
#include "include/llvm-libc-types/ucontext_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include "hdr/types/size_t.h"
#include "include/llvm-libc-macros/signal-macros.h"
#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

#define LIBC_UC_OFFSET(field) __builtin_offsetof(ucontext_t, uc_mcontext.field)
#define LIBC_GREG(n) LIBC_UC_OFFSET(__gregs[n])
#define LIBC_FREG(n) (LIBC_UC_OFFSET(__fpregs) + 8 * (n))
#define LIBC_FCSR (LIBC_UC_OFFSET(__fpregs) + 8 * 32)

__attribute__((naked)) LLVM_LIBC_FUNCTION(int, setcontext,
                                          (const ucontext_t *ucp)) noexcept {
  asm(R"(
      // a0 is ucp. Keep it in t0 across the system call.
      mv t0, a0

      // rt_sigprocmask(SIG_SETMASK, &ucp->uc_sigmask, NULL, sigsetsize)
      addi a1, t0, %c[sigmask]
      li a2, 0
      li a0, %c[sig_setmask]
      li a3, %c[sigset_size]
      li a7, %c[syscall_num]
      ecall

      // The floating point control register, and fs0 to fs11.
      lw t1, %c[fcsr](t0)
      fscsr t1
      fld fs0, %c[f8](t0)
      fld fs1, %c[f9](t0)
      fld fs2, %c[f18](t0)
      fld fs3, %c[f19](t0)
      fld fs4, %c[f20](t0)
      fld fs5, %c[f21](t0)
      fld fs6, %c[f22](t0)
      fld fs7, %c[f23](t0)
      fld fs8, %c[f24](t0)
      fld fs9, %c[f25](t0)
      fld fs10, %c[f26](t0)
      fld fs11, %c[f27](t0)

      // Argument registers, so that a context made by makecontext starts its
      // function with its arguments, and the registers the ABI preserves.
      ld a0, %c[a0](t0)
      ld a1, %c[a1](t0)
      ld a2, %c[a2](t0)
      ld a3, %c[a3](t0)
      ld a4, %c[a4](t0)
      ld a5, %c[a5](t0)
      ld a6, %c[a6](t0)
      ld a7, %c[a7](t0)
      ld s0, %c[s0](t0)
      ld s1, %c[s1](t0)
      ld s2, %c[s2](t0)
      ld s3, %c[s3](t0)
      ld s4, %c[s4](t0)
      ld s5, %c[s5](t0)
      ld s6, %c[s6](t0)
      ld s7, %c[s7](t0)
      ld s8, %c[s8](t0)
      ld s9, %c[s9](t0)
      ld s10, %c[s10](t0)
      ld s11, %c[s11](t0)
      ld ra, %c[ra](t0)
      ld sp, %c[sp](t0)
      ld t1, %c[pc](t0)
      jr t1
      )" ::[sigset_size] "i"(__KERNEL_SIGSET_BYTES),
      [syscall_num] "i"(SYS_rt_sigprocmask), [sig_setmask] "i"(SIG_SETMASK),
      [pc] "i"(LIBC_GREG(0)), [ra] "i"(LIBC_GREG(1)), [sp] "i"(LIBC_GREG(2)),
      [s0] "i"(LIBC_GREG(8)), [s1] "i"(LIBC_GREG(9)), [a0] "i"(LIBC_GREG(10)),
      [a1] "i"(LIBC_GREG(11)), [a2] "i"(LIBC_GREG(12)),
      [a3] "i"(LIBC_GREG(13)), [a4] "i"(LIBC_GREG(14)),
      [a5] "i"(LIBC_GREG(15)), [a6] "i"(LIBC_GREG(16)),
      [a7] "i"(LIBC_GREG(17)), [s2] "i"(LIBC_GREG(18)),
      [s3] "i"(LIBC_GREG(19)), [s4] "i"(LIBC_GREG(20)),
      [s5] "i"(LIBC_GREG(21)), [s6] "i"(LIBC_GREG(22)),
      [s7] "i"(LIBC_GREG(23)), [s8] "i"(LIBC_GREG(24)),
      [s9] "i"(LIBC_GREG(25)), [s10] "i"(LIBC_GREG(26)),
      [s11] "i"(LIBC_GREG(27)), [fcsr] "i"(LIBC_FCSR),
      [f8] "i"(LIBC_FREG(8)), [f9] "i"(LIBC_FREG(9)), [f18] "i"(LIBC_FREG(18)),
      [f19] "i"(LIBC_FREG(19)), [f20] "i"(LIBC_FREG(20)),
      [f21] "i"(LIBC_FREG(21)), [f22] "i"(LIBC_FREG(22)),
      [f23] "i"(LIBC_FREG(23)), [f24] "i"(LIBC_FREG(24)),
      [f25] "i"(LIBC_FREG(25)), [f26] "i"(LIBC_FREG(26)),
      [f27] "i"(LIBC_FREG(27)),
      [sigmask] "i"(__builtin_offsetof(ucontext_t, uc_sigmask))
      : "memory");
}

} // namespace LIBC_NAMESPACE_DECL

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

// The general registers are in the kernel's order: pc, ra, sp, gp, tp, t0-t2,
// s0, s1, a0-a7, s2-s11, t3-t6. The floating point state is the kernel's D
// extension record: f0-f31 as 64 bit values, then fcsr.
#define LIBC_UC_OFFSET(field) __builtin_offsetof(ucontext_t, uc_mcontext.field)
#define LIBC_GREG(n) LIBC_UC_OFFSET(__gregs[n])
#define LIBC_FREG(n) (LIBC_UC_OFFSET(__fpregs) + 8 * (n))
#define LIBC_FCSR (LIBC_UC_OFFSET(__fpregs) + 8 * 32)

// Naked so that the registers are the ones at the call. t0 and the argument
// registers are used, which the call may clobber anyway.
__attribute__((naked)) LLVM_LIBC_FUNCTION(int, getcontext,
                                          (ucontext_t * ucp)) noexcept {
  asm(R"(
      // a0 is ucp. Save the registers the ABI has a function preserve.
      sd ra, %c[pc](a0)
      sd ra, %c[ra](a0)
      sd sp, %c[sp](a0)
      sd s0, %c[s0](a0)
      sd s1, %c[s1](a0)
      sd s2, %c[s2](a0)
      sd s3, %c[s3](a0)
      sd s4, %c[s4](a0)
      sd s5, %c[s5](a0)
      sd s6, %c[s6](a0)
      sd s7, %c[s7](a0)
      sd s8, %c[s8](a0)
      sd s9, %c[s9](a0)
      sd s10, %c[s10](a0)
      sd s11, %c[s11](a0)
      // getcontext returns 0 when it is resumed by setcontext.
      sd zero, %c[a0](a0)

      // The floating point control register, and fs0 to fs11.
      frcsr t0
      sw t0, %c[fcsr](a0)
      fsd fs0, %c[f8](a0)
      fsd fs1, %c[f9](a0)
      fsd fs2, %c[f18](a0)
      fsd fs3, %c[f19](a0)
      fsd fs4, %c[f20](a0)
      fsd fs5, %c[f21](a0)
      fsd fs6, %c[f22](a0)
      fsd fs7, %c[f23](a0)
      fsd fs8, %c[f24](a0)
      fsd fs9, %c[f25](a0)
      fsd fs10, %c[f26](a0)
      fsd fs11, %c[f27](a0)

      // rt_sigprocmask(SIG_BLOCK, NULL, &ucp->uc_sigmask, sigsetsize)
      addi a2, a0, %c[sigmask]
      li a1, 0
      li a0, %c[sig_block]
      li a3, %c[sigset_size]
      li a7, %c[syscall_num]
      ecall

      li a0, 0
      ret
      )" ::[sigset_size] "i"(__KERNEL_SIGSET_BYTES),
      [syscall_num] "i"(SYS_rt_sigprocmask), [sig_block] "i"(SIG_BLOCK),
      [pc] "i"(LIBC_GREG(0)), [ra] "i"(LIBC_GREG(1)), [sp] "i"(LIBC_GREG(2)),
      [s0] "i"(LIBC_GREG(8)), [s1] "i"(LIBC_GREG(9)), [a0] "i"(LIBC_GREG(10)),
      [s2] "i"(LIBC_GREG(18)), [s3] "i"(LIBC_GREG(19)),
      [s4] "i"(LIBC_GREG(20)), [s5] "i"(LIBC_GREG(21)),
      [s6] "i"(LIBC_GREG(22)), [s7] "i"(LIBC_GREG(23)),
      [s8] "i"(LIBC_GREG(24)), [s9] "i"(LIBC_GREG(25)),
      [s10] "i"(LIBC_GREG(26)), [s11] "i"(LIBC_GREG(27)),
      [fcsr] "i"(LIBC_FCSR), [f8] "i"(LIBC_FREG(8)), [f9] "i"(LIBC_FREG(9)),
      [f18] "i"(LIBC_FREG(18)), [f19] "i"(LIBC_FREG(19)),
      [f20] "i"(LIBC_FREG(20)), [f21] "i"(LIBC_FREG(21)),
      [f22] "i"(LIBC_FREG(22)), [f23] "i"(LIBC_FREG(23)),
      [f24] "i"(LIBC_FREG(24)), [f25] "i"(LIBC_FREG(25)),
      [f26] "i"(LIBC_FREG(26)), [f27] "i"(LIBC_FREG(27)),
      [sigmask] "i"(__builtin_offsetof(ucontext_t, uc_sigmask))
      : "memory", "a1", "a2", "a3", "a7", "t0");
}

} // namespace LIBC_NAMESPACE_DECL

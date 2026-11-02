//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of type mcontext_t
///
//===----------------------------------------------------------------------===//

// Note: Definitions in this file are based on the Linux kernel ABI.

#ifndef LLVM_LIBC_TYPES_RISCV_MCONTEXT_T_H
#define LLVM_LIBC_TYPES_RISCV_MCONTEXT_T_H

// This is the kernel's struct sigcontext. The records after the general
// registers (FP/SIMD, SVE and so on) are chained through __reserved.
typedef struct {
  // The kernel's struct sigcontext: the general registers (pc, ra, sp, ...),
  // then the floating point state, 16 byte aligned and padded to the size of
  // the quad precision variant.
  unsigned long __gregs[32];
  unsigned char __fpregs[528] __attribute__((__aligned__(16)));
} mcontext_t;

#endif // LLVM_LIBC_TYPES_RISCV_MCONTEXT_T_H

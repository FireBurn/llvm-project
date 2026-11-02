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

#ifndef LLVM_LIBC_TYPES_AARCH64_MCONTEXT_T_H
#define LLVM_LIBC_TYPES_AARCH64_MCONTEXT_T_H

// This is the kernel's struct sigcontext. The records after the general
// registers (FP/SIMD, SVE and so on) are chained through __reserved.
typedef struct {
  unsigned long long fault_address;
  unsigned long long regs[31];
  unsigned long long sp;
  unsigned long long pc;
  unsigned long long pstate;
  unsigned char __reserved[4096] __attribute__((__aligned__(16)));
} mcontext_t;

#endif // LLVM_LIBC_TYPES_AARCH64_MCONTEXT_T_H

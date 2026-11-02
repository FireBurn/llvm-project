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

// Android programs get the record types that follow the registers
// (_aarch64_ctx, fpsimd_context, esr_context and their magic numbers) from
// <signal.h>, since Bionic includes the kernel header. A signal handler
// cannot walk __reserved without them.
#if defined(__ANDROID__) && defined(__has_include)
#if __has_include(<asm/sigcontext.h>)
#include <asm/sigcontext.h>
#endif
#endif

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

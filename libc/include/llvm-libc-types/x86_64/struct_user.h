//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct user for x86_64.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_X86_64_STRUCT_USER_H
#define LLVM_LIBC_TYPES_X86_64_STRUCT_USER_H

#include "struct_user_fpregs_struct.h"
#include "struct_user_regs_struct.h"

// The layout a core file gave a stopped process, which survives as the thing
// PTRACE_PEEKUSER counts its offsets into.
struct user {
  struct user_regs_struct regs;
  int u_fpvalid;
  struct user_fpregs_struct i387;
  unsigned long u_tsize;
  unsigned long u_dsize;
  unsigned long u_ssize;
  unsigned long start_code;
  unsigned long start_stack;
  long long signal;
  int reserved;
  struct user_regs_struct *u_ar0;
  struct user_fpregs_struct *u_fpstate;
  unsigned long long magic;
  char u_comm[32];
  unsigned long u_debugreg[8];
};

#endif // LLVM_LIBC_TYPES_X86_64_STRUCT_USER_H

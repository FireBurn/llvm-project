//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct user_fpsimd_struct for aarch64.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_AARCH64_STRUCT_USER_FPREGS_STRUCT_H
#define LLVM_LIBC_TYPES_AARCH64_STRUCT_USER_FPREGS_STRUCT_H

// aarch64 has no user_fpregs_struct. Its floating point state is this, which is
// what PTRACE_GETREGSET returns for NT_PRFPREG.
struct user_fpsimd_struct {
  __uint128_t vregs[32];
  unsigned int fpsr;
  unsigned int fpcr;
};

#endif // LLVM_LIBC_TYPES_AARCH64_STRUCT_USER_FPREGS_STRUCT_H

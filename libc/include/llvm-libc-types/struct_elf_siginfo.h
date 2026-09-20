//===-- Definition of struct elf_siginfo ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_ELF_SIGINFO_H
#define LLVM_LIBC_TYPES_STRUCT_ELF_SIGINFO_H

// The signal which killed a process, as a core file records it. This is not
// siginfo_t: only these three fields are written out.
struct elf_siginfo {
  int si_signo;
  int si_code;
  int si_errno;
};

#endif // LLVM_LIBC_TYPES_STRUCT_ELF_SIGINFO_H

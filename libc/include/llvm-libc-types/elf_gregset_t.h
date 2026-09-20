//===-- Definition of elf_gregset_t ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_ELF_GREGSET_T_H
#define LLVM_LIBC_TYPES_ELF_GREGSET_T_H

#include "struct_user_regs_struct.h"

// The general registers as a core file holds them, which is the same block
// ptrace reports, read as an array.
typedef unsigned long elf_greg_t;

#define ELF_NGREG (sizeof(struct user_regs_struct) / sizeof(elf_greg_t))

typedef elf_greg_t elf_gregset_t[ELF_NGREG];

#endif // LLVM_LIBC_TYPES_ELF_GREGSET_T_H

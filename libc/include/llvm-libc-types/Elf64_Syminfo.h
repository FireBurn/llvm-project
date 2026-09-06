//===-- Definition of Elf64_Syminfo type ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_ELF64_SYMINFO_H
#define LLVM_LIBC_TYPES_ELF64_SYMINFO_H

#include "Elf64_Half.h"

// What the syminfo section says about one dynamic symbol.
typedef struct {
  Elf64_Half si_boundto; // The symbol this one is bound directly to.
  Elf64_Half si_flags;
} Elf64_Syminfo;

#endif // LLVM_LIBC_TYPES_ELF64_SYMINFO_H

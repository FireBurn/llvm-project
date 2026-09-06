//===-- Definition of Elf32_Syminfo type ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_ELF32_SYMINFO_H
#define LLVM_LIBC_TYPES_ELF32_SYMINFO_H

#include "Elf32_Half.h"

// What the syminfo section says about one dynamic symbol.
typedef struct {
  Elf32_Half si_boundto; // The symbol this one is bound directly to.
  Elf32_Half si_flags;
} Elf32_Syminfo;

#endif // LLVM_LIBC_TYPES_ELF32_SYMINFO_H

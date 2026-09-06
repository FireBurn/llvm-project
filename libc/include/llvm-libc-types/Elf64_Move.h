//===-- Definition of Elf64_Move type -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_ELF64_MOVE_H
#define LLVM_LIBC_TYPES_ELF64_MOVE_H

#include "Elf64_Half.h"
#include "Elf64_Xword.h"

// One entry of a move table, which fills in a partly initialised object.
typedef struct {
  Elf64_Xword m_value; // The value to move in.
  Elf64_Xword m_info;  // Its size and the symbol it belongs to.
  Elf64_Xword m_poffset;
  Elf64_Half m_repeat;
  Elf64_Half m_stride;
} Elf64_Move;

#endif // LLVM_LIBC_TYPES_ELF64_MOVE_H

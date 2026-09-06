//===-- Definition of Elf32_Move type -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_ELF32_MOVE_H
#define LLVM_LIBC_TYPES_ELF32_MOVE_H

#include "Elf32_Half.h"
#include "Elf32_Word.h"
#include "Elf32_Xword.h"

// One entry of a move table, which fills in a partly initialised object.
typedef struct {
  Elf32_Xword m_value; // The value to move in.
  Elf32_Word m_info;   // Its size and the symbol it belongs to.
  Elf32_Word m_poffset;
  Elf32_Half m_repeat;
  Elf32_Half m_stride;
} Elf32_Move;

#endif // LLVM_LIBC_TYPES_ELF32_MOVE_H

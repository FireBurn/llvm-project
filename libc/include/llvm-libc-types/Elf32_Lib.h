//===-- Definition of Elf32_Lib type --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_ELF32_LIB_H
#define LLVM_LIBC_TYPES_ELF32_LIB_H

#include "Elf32_Word.h"

// One entry of the library list a Solaris object carries.
typedef struct {
  Elf32_Word l_name; // Its name, as an index into the string table.
  Elf32_Word l_time_stamp;
  Elf32_Word l_checksum;
  Elf32_Word l_version;
  Elf32_Word l_flags;
} Elf32_Lib;

#endif // LLVM_LIBC_TYPES_ELF32_LIB_H

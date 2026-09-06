//===-- Definition of Elf64_Lib type --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_ELF64_LIB_H
#define LLVM_LIBC_TYPES_ELF64_LIB_H

#include "Elf64_Word.h"

// One entry of the library list a Solaris object carries.
typedef struct {
  Elf64_Word l_name; // Its name, as an index into the string table.
  Elf64_Word l_time_stamp;
  Elf64_Word l_checksum;
  Elf64_Word l_version;
  Elf64_Word l_flags;
} Elf64_Lib;

#endif // LLVM_LIBC_TYPES_ELF64_LIB_H

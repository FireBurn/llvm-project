//===-- Definition of Elf64_Section type ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_ELF64_SECTION_H
#define LLVM_LIBC_TYPES_ELF64_SECTION_H

#include "../llvm-libc-macros/stdint-macros.h"

// A section header table index, which is what a symbol's st_shndx holds.
typedef uint16_t Elf64_Section;

#endif // LLVM_LIBC_TYPES_ELF64_SECTION_H

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of type struct r_debug.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_R_DEBUG_H
#define LLVM_LIBC_TYPES_STRUCT_R_DEBUG_H

#include "../llvm-libc-macros/link-macros.h"
#include "struct_link_map.h"

// NB: This structure and the related constants are accessed by debuggers and
// similar tools, which have their own definition of the structure. Changing
// their definition here will likely break those tools.
struct r_debug {
  int r_version;
  struct link_map *r_map;
  ElfW(Addr) r_brk;
  // The states are members of the structure, as they are in glibc, so C++
  // callers name them r_debug::RT_CONSISTENT.
  enum { RT_CONSISTENT = 0, RT_ADD = 1, RT_DELETE = 2 };
  int r_state;
  ElfW(Addr) r_ldbase;
};

#endif // LLVM_LIBC_TYPES_STRUCT_R_DEBUG_H

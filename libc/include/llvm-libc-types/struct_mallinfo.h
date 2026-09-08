//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct mallinfo and struct mallinfo2.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_MALLINFO_H
#define LLVM_LIBC_TYPES_STRUCT_MALLINFO_H

#include "size_t.h"

// What the allocator has taken from the system and what it is doing with
// it. The older form counts in ints, which overflow on a large heap, which
// is why the second one exists.
struct mallinfo {
  int arena;    // Space taken from the system, mappings aside.
  int ordblks;  // Free chunks.
  int smblks;   // Free chunks on the fast lists.
  int hblks;    // Mappings made for large allocations.
  int hblkhd;   // Space in those mappings.
  int usmblks;  // Always zero; it is kept for the older shape of this.
  int fsmblks;  // Space on the fast lists.
  int uordblks; // Space handed out.
  int fordblks; // Space free.
  int keepcost; // The part at the top which could be given back.
};

#endif // LLVM_LIBC_TYPES_STRUCT_MALLINFO_H

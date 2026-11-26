//===-- Object with packed relocations
//--------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// Needs relative and symbolic relocations, which are linked packed in
/// Android's form. See packed_relocs_test.cpp.
///
//===----------------------------------------------------------------------===//

extern "C" {

// Defined by the program, which exports it.
extern int llvm_libc_packed_values[4];

static int llvm_libc_packed_local[4] = {1, 2, 3, 4};

// Pointers into this object take relative relocations, those into the program
// symbolic ones, the later of each with an addend.
static int *llvm_libc_packed_pointers[] = {
    &llvm_libc_packed_local[0],  &llvm_libc_packed_local[1],
    &llvm_libc_packed_local[2],  &llvm_libc_packed_local[3],
    &llvm_libc_packed_values[0], &llvm_libc_packed_values[1],
    &llvm_libc_packed_values[2], &llvm_libc_packed_values[3],
};

int llvm_libc_packed_sum() {
  int sum = 0;
  for (int *volatile pointer : llvm_libc_packed_pointers)
    sum += *pointer;
  return sum;
}
}

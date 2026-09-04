//===-- Definition of struct shminfo --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SHMINFO_H
#define LLVM_LIBC_TYPES_STRUCT_SHMINFO_H

// The limits shmctl reports for IPC_INFO. This is the wide layout the
// kernel fills in, which it calls shminfo64.
struct shminfo {
  unsigned long shmmax; // The largest a segment may be.
  unsigned long shmmin; // The smallest.
  unsigned long shmmni; // How many segments there may be at once.
  unsigned long shmseg; // How many one process may have attached.
  unsigned long shmall; // How many pages of shared memory in all.
  unsigned long __unused1;
  unsigned long __unused2;
  unsigned long __unused3;
  unsigned long __unused4;
};

#endif // LLVM_LIBC_TYPES_STRUCT_SHMINFO_H

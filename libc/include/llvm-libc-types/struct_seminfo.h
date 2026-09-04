//===-- Definition of struct seminfo --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SEMINFO_H
#define LLVM_LIBC_TYPES_STRUCT_SEMINFO_H

// The limits semctl reports for IPC_INFO, and, with the members standing for
// something else, the counts it reports for SEM_INFO.
struct seminfo {
  int semmap; // Entries in the semaphore map.
  int semmni; // How many semaphore sets there may be at once.
  int semmns; // How many semaphores in all.
  int semmnu; // How many undo structures.
  int semmsl; // How many semaphores one set may hold.
  int semopm; // How many operations one semop call may make.
  int semume; // How many undo entries one process may have.
  int semusz; // The size of an undo structure.
  int semvmx; // The largest value a semaphore may take.
  int semaem; // The largest an undo entry may record.
};

#endif // LLVM_LIBC_TYPES_STRUCT_SEMINFO_H

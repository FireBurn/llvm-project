//===-- Definition of struct shmid_ds -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SHMID_DS_H
#define LLVM_LIBC_TYPES_STRUCT_SHMID_DS_H

#include "pid_t.h"
#include "size_t.h"
#include "struct_ipc_perm.h"
#include "time_t.h"

// What shmctl reports about a shared memory segment. The layout is the
// kernel's.
struct shmid_ds {
  struct ipc_perm shm_perm;
  size_t shm_segsz; // How large the segment is, in bytes.
  time_t shm_atime; // When it was last attached.
#if defined(__linux__) && __SIZEOF_LONG__ == 4
  // Where a long is too narrow to hold the time, the kernel keeps the top
  // half of each one in a word of its own after it.
  unsigned long __shm_atime_high;
#endif
  time_t shm_dtime; // When it was last detached.
#if defined(__linux__) && __SIZEOF_LONG__ == 4
  unsigned long __shm_dtime_high;
#endif
  time_t shm_ctime; // When it was last changed.
#if defined(__linux__) && __SIZEOF_LONG__ == 4
  unsigned long __shm_ctime_high;
#endif
  pid_t shm_cpid;           // Who created it.
  pid_t shm_lpid;           // Who last attached or detached it.
  unsigned long shm_nattch; // How many have it attached now.
#ifdef __linux__
  unsigned long __unused4;
  unsigned long __unused5;
#endif
};

#endif // LLVM_LIBC_TYPES_STRUCT_SHMID_DS_H

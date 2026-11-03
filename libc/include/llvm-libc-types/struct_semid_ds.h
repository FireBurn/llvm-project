//===-- Definition of struct semid_ds -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SEMID_DS_H
#define LLVM_LIBC_TYPES_STRUCT_SEMID_DS_H

#include "struct_ipc_perm.h"
#include "time_t.h"

struct semid_ds {
  struct ipc_perm sem_perm;
#if defined(__linux__) && defined(__x86_64__)
  // x86_64 keeps a word of padding after each time.
  time_t sem_otime;
  unsigned long __unused1;
  time_t sem_ctime;
  unsigned long __unused2;
#elif defined(__linux__) && __SIZEOF_LONG__ == 4
  // Where a long is too narrow to hold the time, the kernel keeps the top
  // half of each one in a word of its own after it.
  time_t sem_otime;
  unsigned long __sem_otime_high;
  time_t sem_ctime;
  unsigned long __sem_ctime_high;
#else
  time_t sem_otime;
  time_t sem_ctime;
#endif
  unsigned long sem_nsems;
#ifdef __linux__
  unsigned long __unused3;
  unsigned long __unused4;
#endif
};

#endif // LLVM_LIBC_TYPES_STRUCT_SEMID_DS_H

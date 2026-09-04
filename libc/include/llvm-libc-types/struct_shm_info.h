//===-- Definition of struct shm_info -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SHM_INFO_H
#define LLVM_LIBC_TYPES_STRUCT_SHM_INFO_H

// What shmctl reports for SHM_INFO: how much shared memory is in use rather
// than what the limits are.
struct shm_info {
  int used_ids;                // How many segments exist.
  unsigned long shm_tot;       // Pages allocated in all.
  unsigned long shm_rss;       // Pages which are resident.
  unsigned long shm_swp;       // Pages which are swapped out.
  unsigned long swap_attempts; // No longer counted; kept for the layout.
  unsigned long swap_successes;
};

#endif // LLVM_LIBC_TYPES_STRUCT_SHM_INFO_H

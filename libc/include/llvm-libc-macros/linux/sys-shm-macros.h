//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux specific declarations of macros from sys/shm.h.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_SYS_SHM_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_SYS_SHM_MACROS_H

#include "../../__llvm-libc-common.h"
#include "../stdint-macros.h"

// The permission bits a segment is created with.
#define SHM_R 0400
#define SHM_W 0200

// shmat flags
#define SHM_RDONLY 010000
#define SHM_RND 020000
#define SHM_REMAP 040000
#define SHM_EXEC 0100000

// The shmctl commands which are Linux's own.
#define SHM_LOCK 11   // Keep the segment out of swap.
#define SHM_UNLOCK 12 // Let it be swapped again.
#define SHM_STAT 13
#define SHM_INFO 14
#define SHM_STAT_ANY 15

// The mode bits shmctl reports which are not permissions.
#define SHM_DEST 01000    // Destroy the segment when the last one detaches.
#define SHM_LOCKED 02000  // The segment is kept out of swap.
#define SHM_HUGETLB 04000 // It is backed by huge pages.
#define SHM_NORESERVE 010000

// What an attach address is rounded to when SHM_RND asks for it.
#define SHMLBA 4096
#define SHM_FAILED                                                             \
  __LLVM_LIBC_CAST(reinterpret_cast, void *,                                   \
                   __LLVM_LIBC_CAST(static_cast, intptr_t, -1))

#endif // LLVM_LIBC_MACROS_LINUX_SYS_SHM_MACROS_H

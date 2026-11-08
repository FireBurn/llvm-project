//===-- Definition of macros to be used with malloc functions -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_MALLOC_MACROS_H
#define LLVM_LIBC_MACROS_MALLOC_MACROS_H

// Note: these values only make sense when Scudo is used as the memory
// allocator.
#define M_PURGE (-101)
#define M_PURGE_ALL (-104)

// The rest of the parameters Scudo's mallopt takes, at its numbers. Bionic has
// the same ones, and Android programs name them.
#define M_DECAY_TIME (-100)
#define M_MEMTAG_TUNING (-102)
#define M_THREAD_DISABLE_MEM_INIT (-103)
#define M_PURGE_FAST (-105)
#define M_CACHE_COUNT_MAX (-200)
#define M_CACHE_SIZE_MAX (-201)
#define M_TSDS_COUNT_MAX (-202)
#define M_LOG_STATS (-205)
#define M_CACHE_RESIDENT_BYTES_MAX (-206)

// The values M_MEMTAG_TUNING takes.
#define M_MEMTAG_TUNING_BUFFER_OVERFLOW 0
#define M_MEMTAG_TUNING_UAF 1

// The mallopt parameters of glibc's malloc.h, at its numbers. Scudo takes
// none of them and mallopt reports that it did not set the option.
#define M_MXFAST 1
#define M_NLBLKS 2
#define M_GRAIN 3
#define M_KEEP 4
#define M_TRIM_THRESHOLD (-1)
#define M_TOP_PAD (-2)
#define M_MMAP_THRESHOLD (-3)
#define M_MMAP_MAX (-4)
#define M_CHECK_ACTION (-5)
#define M_PERTURB (-6)
#define M_ARENA_TEST (-7)
#define M_ARENA_MAX (-8)

#endif // LLVM_LIBC_MACROS_MALLOC_MACROS_H

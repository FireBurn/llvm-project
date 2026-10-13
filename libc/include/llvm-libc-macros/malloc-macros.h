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

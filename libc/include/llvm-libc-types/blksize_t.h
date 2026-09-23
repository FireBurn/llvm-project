//===-- Definition of blksize_t type --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_BLKSIZE_T_H
#define LLVM_LIBC_TYPES_BLKSIZE_T_H

// The kernel's width: a pointer's on x86_64, 32 bits elsewhere.
#if defined(__x86_64__)
typedef __INTPTR_TYPE__ blksize_t;
#else
typedef int blksize_t;
#endif

#endif // LLVM_LIBC_TYPES_BLKSIZE_T_H

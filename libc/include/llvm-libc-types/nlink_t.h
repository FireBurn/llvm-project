//===-- Definition of nlink_t type ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_NLINK_T_H
#define LLVM_LIBC_TYPES_NLINK_T_H

// The kernel's width: a pointer's on x86_64, 32 bits elsewhere.
#if defined(__x86_64__)
typedef __UINTPTR_TYPE__ nlink_t;
#else
typedef unsigned int nlink_t;
#endif

#endif // LLVM_LIBC_TYPES_NLINK_T_H

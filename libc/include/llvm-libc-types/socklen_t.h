//===-- Definition of socklen_t type ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_SOCKLEN_T_H
#define LLVM_LIBC_TYPES_SOCKLEN_T_H

// POSIX only requires an integer type of at least 32 bits, but socklen_t is
// passed to and written by the kernel, which uses a 32 bit int, and it is 32
// bits in every other Linux libc.
typedef unsigned int socklen_t;

#endif // LLVM_LIBC_TYPES_SOCKLEN_T_H

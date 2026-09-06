//===-- Definition of thrd_t type -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES___THREAD_TYPE_H
#define LLVM_LIBC_TYPES___THREAD_TYPE_H

// A thread is named by a pointer to what the library keeps about it, stated
// as an integer wide enough to hold one. POSIX allows a structure here, but
// every other C library makes this a scalar and a great deal of software
// takes that for granted: it compares two of them, tests one against zero,
// or prints one. The width and meaning are the same either way.
typedef unsigned long __thread_type;

#endif // LLVM_LIBC_TYPES___THREAD_TYPE_H

//===-- Macros defined in threads.h header file ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_THREADS_MACROS_H
#define LLVM_LIBC_MACROS_THREADS_MACROS_H

#define ONCE_FLAG_INIT {0}
#define TSS_DTOR_ITERATIONS 4

// C23 made this a keyword. Before that it is spelled with an underscore and
// the plain name is a macro, which is what code written against C11 uses.
#if !defined(__cplusplus) &&                                                   \
    (!defined(__STDC_VERSION__) || __STDC_VERSION__ <= 201710L)
#define thread_local _Thread_local
#endif

#endif // LLVM_LIBC_MACROS_THREADS_MACROS_H

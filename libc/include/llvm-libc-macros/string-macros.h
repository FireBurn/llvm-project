//===-- Macros defined in string.h header file ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_STRING_MACROS_H
#define LLVM_LIBC_MACROS_STRING_MACROS_H

// A copy of a string on the stack, which goes away when the calling function
// returns. Both are statement expressions, so they are only available where
// the compiler has that extension.
#if defined(__GNUC__) || defined(__clang__)

#define strdupa(s)                                                             \
  (__extension__({                                                             \
    const char *__old = (s);                                                   \
    __SIZE_TYPE__ __len = strlen(__old) + 1;                                   \
    char *__new = (char *)__builtin_alloca(__len);                             \
    (char *)memcpy(__new, __old, __len);                                       \
  }))

// At most `n` bytes of it, always terminated.
#define strndupa(s, n)                                                         \
  (__extension__({                                                             \
    const char *__old = (s);                                                   \
    __SIZE_TYPE__ __len = strnlen(__old, (n));                                 \
    char *__new = (char *)__builtin_alloca(__len + 1);                         \
    __new[__len] = '\0';                                                       \
    (char *)memcpy(__new, __old, __len);                                       \
  }))

#endif // defined(__GNUC__) || defined(__clang__)

// There are two strerror_r. The one POSIX states fills the caller's buffer
// and answers with an error number; the one _GNU_SOURCE asks for answers
// with a string which may not be that buffer at all. They are separate
// functions, and this is the name the POSIX one goes by, as it does in
// glibc.
#ifndef _GNU_SOURCE
#define strerror_r __xpg_strerror_r
#endif

#endif // LLVM_LIBC_MACROS_STRING_MACROS_H

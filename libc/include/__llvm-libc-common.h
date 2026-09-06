//===-- Common definitions for LLVM-libc public header files --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM_LIBC_COMMON_H
#define _LLVM_LIBC_COMMON_H

#define __LLVM_LIBC__ 1

// Feature test macros. An application selects a view of the library by
// defining one of these before including anything; where it has not, or
// where it asked for all of them at once, the rest of the set is settled
// here so that a program reading them back sees a consistent answer.
#ifdef _GNU_SOURCE
#undef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 202405L
#undef _XOPEN_SOURCE
#define _XOPEN_SOURCE 800
#undef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif

#if defined _DEFAULT_SOURCE ||                                                 \
    (!defined __STRICT_ANSI__ && !defined _ISOC95_SOURCE &&                    \
     !defined _ISOC99_SOURCE && !defined _ISOC11_SOURCE &&                     \
     !defined _ISOC23_SOURCE && !defined _ISOC2X_SOURCE &&                     \
     !defined _POSIX_SOURCE && !defined _POSIX_C_SOURCE &&                     \
     !defined _XOPEN_SOURCE)
#undef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#undef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 202405L
#endif

#ifdef __cplusplus

#undef __BEGIN_C_DECLS
#define __BEGIN_C_DECLS extern "C" {

#undef __END_C_DECLS
#define __END_C_DECLS }

// Standard C++ doesn't have C99 restrict but GNU C++ has it with __ spelling.
#undef __restrict
#ifndef __GNUC__
#define __restrict
#endif

// The C spellings map onto the C++ keywords, which only C++11 onwards has.
// Before that the compiler extensions they were built on stand in, so a
// header carrying one of them still compiles as C++98.
#undef _Noreturn
#undef _Alignas
#undef _Static_assert
#undef _Alignof
#if __cplusplus >= 201103L
#define _Noreturn [[noreturn]]
#define _Alignas alignas
#define _Static_assert static_assert
#define _Alignof alignof
#else
#define _Noreturn __attribute__((noreturn))
#define _Alignas(x) __attribute__((aligned(x)))
#define _Alignof(x) __alignof__(x)
#define __LLVM_LIBC_STATIC_ASSERT_JOIN(a, b) a##b
#define __LLVM_LIBC_STATIC_ASSERT_NAME(line)                                   \
  __LLVM_LIBC_STATIC_ASSERT_JOIN(__llvm_libc_static_assert_, line)
#define _Static_assert(expr, msg)                                              \
  typedef char __LLVM_LIBC_STATIC_ASSERT_NAME(__LINE__)[(expr) ? 1 : -1]
#endif

#undef __NOEXCEPT
#if __cplusplus >= 201103L
#define __NOEXCEPT noexcept
#else
#define __NOEXCEPT throw()
#endif

#undef _Returns_twice
#if __cplusplus >= 201103L
#define _Returns_twice [[gnu::returns_twice]]
#else
#define _Returns_twice __attribute__((returns_twice))
#endif

// This macro serves as a generic cast implementation for use in both C and C++,
// similar to `__BIONIC_CAST` in Android.
#undef __LLVM_LIBC_CAST
#define __LLVM_LIBC_CAST(cast, type, value) (cast<type>(value))

#else // not __cplusplus

#undef __BEGIN_C_DECLS
#define __BEGIN_C_DECLS

#undef __END_C_DECLS
#define __END_C_DECLS

#undef __restrict
#if __STDC_VERSION__ >= 199901L
// C99 and above support the restrict keyword.
#define __restrict restrict
#elif !defined(__GNUC__)
// GNU-compatible compilers accept the __ spelling in all modes.
// Otherwise, omit the qualifier for pure C89 compatibility.
#define __restrict
#endif

#undef _Noreturn
#if __STDC_VERSION__ >= 201112L
// In C11 and later, _Noreturn is a keyword.
#elif defined(__GNUC__)
// GNU-compatible compilers have an equivalent attribute.
#define _Noreturn __attribute__((__noreturn__))
#else
#define _Noreturn
#endif

#undef __NOEXCEPT
#ifdef __GNUC__
#define __NOEXCEPT __attribute__((__nothrow__))
#else
#define __NOEXCEPT
#endif

#undef _Returns_twice
#define _Returns_twice __attribute__((returns_twice))

#undef __LLVM_LIBC_CAST
#define __LLVM_LIBC_CAST(cast, type, value) ((type)(value))

#endif // __cplusplus

#endif // _LLVM_LIBC_COMMON_H

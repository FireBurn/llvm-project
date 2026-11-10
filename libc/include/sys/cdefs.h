//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// glibc's sys/cdefs.h
///
//===----------------------------------------------------------------------===//

#ifndef _SYS_CDEFS_H
#define _SYS_CDEFS_H

// The macros glibc's headers are written with, which programs written against
// them use in declarations of their own. Only the ones in common use are
// here, and none of the machinery glibc builds its own headers from.

#include "../__llvm-libc-common.h"

#ifndef __BEGIN_DECLS
#define __BEGIN_DECLS __BEGIN_C_DECLS
#endif
#ifndef __END_DECLS
#define __END_DECLS __END_C_DECLS
#endif

#ifndef __THROW
#define __THROW __NOEXCEPT
#endif
#ifndef __THROWNL
#define __THROWNL __NOEXCEPT
#endif
#ifndef __NTH
#define __NTH(fct) fct __NOEXCEPT
#endif
#ifndef __NTHNL
#define __NTHNL(fct) fct __NOEXCEPT
#endif

#ifndef __P
#define __P(args) args
#endif
#ifndef __PMT
#define __PMT(args) args
#endif
#ifndef __CONCAT
#define __CONCAT(x, y) x##y
#endif
#ifndef __STRING
#define __STRING(x) #x
#endif
#ifndef __ptr_t
#define __ptr_t void *
#endif

#ifndef __GNUC_PREREQ
#define __GNUC_PREREQ(maj, min)                                                \
  ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#endif
#ifndef __glibc_clang_prereq
#define __glibc_clang_prereq(maj, min)                                         \
  ((__clang_major__ << 16) + __clang_minor__ >= ((maj) << 16) + (min))
#endif
#ifndef __glibc_has_attribute
#define __glibc_has_attribute(attr) __has_attribute(attr)
#endif
#ifndef __glibc_has_builtin
#define __glibc_has_builtin(name) __has_builtin(name)
#endif

#ifndef __glibc_unlikely
#define __glibc_unlikely(cond) __builtin_expect((cond), 0)
#endif
#ifndef __glibc_likely
#define __glibc_likely(cond) __builtin_expect((cond), 1)
#endif

#ifndef __attribute_malloc__
#define __attribute_malloc__ __attribute__((__malloc__))
#endif
#ifndef __attribute_pure__
#define __attribute_pure__ __attribute__((__pure__))
#endif
#ifndef __attribute_const__
#define __attribute_const__ __attribute__((__const__))
#endif
#ifndef __attribute_used__
#define __attribute_used__ __attribute__((__used__))
#endif
#ifndef __attribute_noinline__
#define __attribute_noinline__ __attribute__((__noinline__))
#endif
#ifndef __attribute_deprecated__
#define __attribute_deprecated__ __attribute__((__deprecated__))
#endif
#ifndef __attribute_deprecated_msg__
#define __attribute_deprecated_msg__(msg) __attribute__((__deprecated__(msg)))
#endif
#ifndef __attribute_format_arg__
#define __attribute_format_arg__(x) __attribute__((__format_arg__(x)))
#endif
#ifndef __attribute_format_strfmon__
#define __attribute_format_strfmon__(a, b)                                     \
  __attribute__((__format__(__strfmon__, a, b)))
#endif
#ifndef __attribute_warn_unused_result__
#define __attribute_warn_unused_result__ __attribute__((__warn_unused_result__))
#endif
#ifndef __attribute_alloc_size__
#define __attribute_alloc_size__(params) __attribute__((__alloc_size__ params))
#endif
#ifndef __attribute_artificial__
#define __attribute_artificial__ __attribute__((__artificial__))
#endif
#ifndef __returns_nonnull
#define __returns_nonnull __attribute__((__returns_nonnull__))
#endif
#ifndef __nonnull
#define __nonnull(params) __attribute__((__nonnull__ params))
#endif
#ifndef __wur
#define __wur __attribute__((__warn_unused_result__))
#endif

#ifndef __always_inline
#define __always_inline __inline __attribute__((__always_inline__))
#endif
#ifndef __extern_inline
#define __extern_inline extern __inline __attribute__((__gnu_inline__))
#endif
#ifndef __extern_always_inline
#define __extern_always_inline                                                 \
  extern __always_inline __attribute__((__gnu_inline__))
#endif
#ifndef __fortify_function
#define __fortify_function __extern_always_inline __attribute_artificial__
#endif

#ifndef __restrict_arr
#define __restrict_arr __restrict
#endif
#ifndef __flexarr
#define __flexarr []
#endif
#ifndef __glibc_c99_flexarr_available
#define __glibc_c99_flexarr_available 1
#endif

#ifdef __ANDROID__
// Android headers mark each declaration with the API level that introduced it.
// Everything here exists at every level, so the marks have no effect.
#ifndef __BIONIC_AVAILABILITY
#define __BIONIC_AVAILABILITY(__what, ...)
#endif
#ifndef __BIONIC_AVAILABILITY_GUARD
#define __BIONIC_AVAILABILITY_GUARD(api_level) 1
#endif
#ifndef __INTRODUCED_IN
#define __INTRODUCED_IN(api_level)
#endif
#ifndef __DEPRECATED_IN
#define __DEPRECATED_IN(api_level, msg)
#endif
#ifndef __REMOVED_IN
#define __REMOVED_IN(api_level, msg)
#endif
#ifndef __INTRODUCED_IN_32
#define __INTRODUCED_IN_32(api_level)
#endif
#ifndef __INTRODUCED_IN_64
#define __INTRODUCED_IN_64(api_level)
#endif

// bionic's names for giving a declaration another symbol, which the NDK
// headers use for the calls that take an off_t. Every off_t is 64 bits wide
// here, so the second one never renames.
#ifndef __RENAME
#define __RENAME(x) __asm__(#x)
#endif
#ifndef __RENAME_IF_FILE_OFFSET64
#define __RENAME_IF_FILE_OFFSET64(func)
#endif

// Android code expects the API level macros from any header, and the kernel's
// fixed width types, off64_t and the fcntl constants that bionic's
// <sys/types.h>, <sys/socket.h> and <unistd.h> bring with them.
#include <android/api-level.h>
#include <linux/posix_types.h>
// bionic's <time.h> brings in struct __kernel_timespec, which liburing and
// other users of the kernel's time interfaces expect.
#include <linux/time_types.h>
#include <linux/types.h>
#include "../llvm-libc-macros/fcntl-macros.h"
// bionic's <sys/time.h> brings in the CLOCK_* ids through <linux/time.h>.
#include "../llvm-libc-macros/time-macros.h"
#include "../llvm-libc-types/off64_t.h"

// bionic's <sys/cdefs.h> defines this for the code written against it.
#ifndef __unused
#define __unused __attribute__((__unused__))
#endif

// The attribute names bionic's <sys/cdefs.h> provides that glibc's does not.
#ifndef __noreturn
#define __noreturn __attribute__((__noreturn__))
#endif
#ifndef __dead
#define __dead __attribute__((__noreturn__))
#endif
#ifndef __packed
#define __packed __attribute__((__packed__))
#endif
#ifndef __returns_twice
#define __returns_twice __attribute__((__returns_twice__))
#endif
#ifndef __used
#define __used __attribute__((__used__))
#endif
#ifndef __mallocfunc
#define __mallocfunc __attribute__((__malloc__))
#endif
#ifndef __nodiscard
#define __nodiscard __attribute__((__warn_unused_result__))
#endif

// The format attributes bionic's <sys/cdefs.h> provides.
#ifndef __printflike
#define __printflike(x, y) __attribute__((__format__(__printf__, x, y)))
#endif
#ifndef __scanflike
#define __scanflike(x, y) __attribute__((__format__(__scanf__, x, y)))
#endif
#ifndef __strftimelike
#define __strftimelike(x) __attribute__((__format__(__strftime__, x, 0)))
#endif

// The helpers bionic's <sys/cdefs.h> provides for its own and for platform
// code, which uses them without declaring anything else.
#ifndef __strong_alias
// Gives a symbol defined in the same object file a second, global name.
#define __strong_alias(alias, sym) \
  __asm__(".global " #alias "\n" #alias " = " #sym);
#endif
#ifndef __BIONIC_CAST
#ifdef __cplusplus
#define __BIONIC_CAST(_k, _t, _v) (_k<_t>(_v))
#else
#define __BIONIC_CAST(_k, _t, _v) ((_t)(_v))
#endif
#endif
#ifndef __BIONIC_COMPLICATED_NULLNESS
#define __BIONIC_COMPLICATED_NULLNESS _Null_unspecified
#endif
#ifndef __predict_true
#define __predict_true(exp) __builtin_expect((exp) != 0, 1)
#endif
#ifndef __predict_false
#define __predict_false(exp) __builtin_expect((exp) != 0, 0)
#endif

// Android's own headers mark what the library keeps to itself with this.
#ifndef __LIBC_HIDDEN__
#define __LIBC_HIDDEN__ __attribute__((__visibility__("hidden")))
#endif

#endif // __ANDROID__

#endif // _SYS_CDEFS_H

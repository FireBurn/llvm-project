//===-- glibc's sys/cdefs.h -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
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

#endif // _SYS_CDEFS_H

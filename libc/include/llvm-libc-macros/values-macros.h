//===-- Macros defined in values.h header file ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_VALUES_MACROS_H
#define LLVM_LIBC_MACROS_VALUES_MACROS_H

#include <float.h>
#include <limits.h>

// The names System V gave these limits, kept for code old enough to ask for
// them. Each one stands for the <limits.h> or <float.h> name beside it.

#define BITSPERBYTE CHAR_BIT
#define SHORTBITS ((int)(sizeof(short) * CHAR_BIT))
#define INTBITS ((int)(sizeof(int) * CHAR_BIT))
#define LONGBITS ((int)(sizeof(long) * CHAR_BIT))
#define PTRBITS ((int)(sizeof(void *) * CHAR_BIT))
#define DOUBLEBITS ((int)(sizeof(double) * CHAR_BIT))
#define FLOATBITS ((int)(sizeof(float) * CHAR_BIT))

#define MINSHORT SHRT_MIN
#define MININT INT_MIN
#define MINLONG LONG_MIN

#define MAXSHORT SHRT_MAX
#define MAXINT INT_MAX
#define MAXLONG LONG_MAX

#define HIBITS MINSHORT
#define HIBITL MINLONG

#define MAXDOUBLE DBL_MAX
#define MAXFLOAT FLT_MAX
#define MINDOUBLE DBL_MIN
#define MINFLOAT FLT_MIN

#define DMINEXP DBL_MIN_EXP
#define FMINEXP FLT_MIN_EXP
#define DMAXEXP DBL_MAX_EXP
#define FMAXEXP FLT_MAX_EXP

// How many bits a type takes up, which is what the header was written for.
#define BITS(type) ((int)(sizeof(type) * CHAR_BIT))

#endif // LLVM_LIBC_MACROS_VALUES_MACROS_H

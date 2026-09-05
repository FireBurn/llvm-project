//===-- Macros defined in printf.h header file ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_PRINTF_MACROS_H
#define LLVM_LIBC_MACROS_PRINTF_MACROS_H

// The kind of argument a conversion takes, as parse_printf_format reports
// it. The low byte is one of these.
#define PA_INT 0
#define PA_CHAR 1
#define PA_WCHAR 2
#define PA_STRING 3
#define PA_WSTRING 4
#define PA_POINTER 5
#define PA_FLOAT 6
#define PA_DOUBLE 7
#define PA_LAST 8

// The high byte says how wide it is, and whether the conversion takes a
// pointer to one rather than the thing itself.
#define PA_FLAG_MASK 0xff00
#define PA_FLAG_LONG_LONG (1 << 8)
#define PA_FLAG_LONG_DOUBLE PA_FLAG_LONG_LONG
#define PA_FLAG_LONG (1 << 9)
#define PA_FLAG_SHORT (1 << 10)
#define PA_FLAG_PTR (1 << 11)

#endif // LLVM_LIBC_MACROS_PRINTF_MACROS_H

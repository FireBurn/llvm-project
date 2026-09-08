//===-- Macros defined in stdio_ext.h header file -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_STDIO_EXT_MACROS_H
#define LLVM_LIBC_MACROS_STDIO_EXT_MACROS_H

// Who takes a stream's lock, which is what __fsetlocking sets and reads.
#define FSETLOCKING_QUERY 0
#define FSETLOCKING_INTERNAL 1
#define FSETLOCKING_BYCALLER 2

#endif // LLVM_LIBC_MACROS_STDIO_EXT_MACROS_H

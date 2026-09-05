//===-- Definition of macros from ar.h ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_AR_MACROS_H
#define LLVM_LIBC_MACROS_AR_MACROS_H

// What an archive begins with, and how long that is. There is no terminator
// in the file, so the length is given rather than found.
#define ARMAG "!<arch>\n"
#define SARMAG 8

// What the last field of every member's header holds.
#define ARFMAG "`\n"

#endif // LLVM_LIBC_MACROS_AR_MACROS_H

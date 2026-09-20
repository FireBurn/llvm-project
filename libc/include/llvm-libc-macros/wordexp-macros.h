//===-- Macros defined in wordexp.h header file ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_WORDEXP_MACROS_H
#define LLVM_LIBC_MACROS_WORDEXP_MACROS_H

// What the caller may ask for.
#define WRDE_DOOFFS 0x0001  // Leave we_offs slots empty at the front.
#define WRDE_APPEND 0x0002  // Add to what is in we_wordv already.
#define WRDE_NOCMD 0x0004   // Refuse command substitution.
#define WRDE_REUSE 0x0008   // we_wordv came from an earlier call; reuse it.
#define WRDE_SHOWERR 0x0010 // Let the expansion write to stderr.
#define WRDE_UNDEF 0x0020   // An unset variable is an error.

// What it may come back with.
#define WRDE_NOSPACE 1 // Out of memory; we_wordv is still usable.
#define WRDE_BADCHAR 2 // A character the expansion will not take.
#define WRDE_BADVAL 3  // An unset variable, with WRDE_UNDEF asked for.
#define WRDE_CMDSUB 4  // Command substitution, which this does not do.
#define WRDE_SYNTAX 5  // The words do not parse.

#endif // LLVM_LIBC_MACROS_WORDEXP_MACROS_H

//===-- Macros defined in stdio.h header file -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_STDIO_MACROS_H
#define LLVM_LIBC_MACROS_STDIO_MACROS_H

#include "../llvm-libc-types/FILE.h"

#ifdef __cplusplus
extern "C" FILE *stdin;
extern "C" FILE *stdout;
extern "C" FILE *stderr;
#else
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;
#endif

#ifndef stdin
#define stdin stdin
#endif

#ifndef stdout
#define stdout stdout
#endif

#ifndef stderr
#define stderr stderr
#endif

#ifndef EOF
#define EOF (-1)
#endif

#define BUFSIZ 1024

// The C standard requires these but LLVM-libc did not define them, which
// meant a translation unit could compile against stdio.h and still fail on
// any of the four. Values follow the minimums the standard sets, except
// FILENAME_MAX, which matches the longest path the kernel accepts.

// The size an array must have to hold the longest supported file name.
#ifndef FILENAME_MAX
#define FILENAME_MAX 4096
#endif

// The minimum number of files that can be open at once. The standard's floor
// is 8; the real limit is whatever RLIMIT_NOFILE allows.
#ifndef FOPEN_MAX
#define FOPEN_MAX 16
#endif

// The size an array must have to hold a string returned by tmpnam.
#ifndef L_tmpnam
#define L_tmpnam 20
#endif

// The minimum number of distinct names tmpnam can generate. The standard's
// floor is 25.
#ifndef TMP_MAX
#define TMP_MAX 238328
#endif

#define _IONBF 2
#define _IOLBF 1
#define _IOFBF 0

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif

#ifndef SEEK_END
#define SEEK_END 2
#endif

// What renameat2 may be asked to do instead of a plain rename. glibc
// declares these in <stdio.h> too.
#define RENAME_NOREPLACE (1 << 0)
#define RENAME_EXCHANGE (1 << 1)
#define RENAME_WHITEOUT (1 << 2)

#endif // LLVM_LIBC_MACROS_STDIO_MACROS_H

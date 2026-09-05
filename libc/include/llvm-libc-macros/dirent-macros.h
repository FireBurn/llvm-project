//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of macros from dirent.h.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_DIRENT_MACROS_H
#define LLVM_LIBC_MACROS_DIRENT_MACROS_H

#ifdef __linux__
#include "linux/dirent-macros.h"
#endif

// LLVM-libc has one set of interfaces, and they are already the wide ones,
// so the names a program uses to ask for large file support name the same
// things. musl does the same.
#if defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)
#define dirent64 dirent
#define readdir64 readdir
#define readdir64_r readdir_r
#define scandir64 scandir
#define alphasort64 alphasort
#define versionsort64 versionsort
#endif

#endif // LLVM_LIBC_MACROS_DIRENT_MACROS_H

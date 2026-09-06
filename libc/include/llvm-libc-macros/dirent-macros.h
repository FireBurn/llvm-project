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

#include "lfs64-macros.h"

// The BSD name for what POSIX calls NAME_MAX, which <limits.h> gives.
#ifndef MAXNAMLEN
#ifdef NAME_MAX
#define MAXNAMLEN NAME_MAX
#else
#define MAXNAMLEN 255
#endif
#endif

// The name large file support gave struct dirent. LLVM-libc's ordinary
// record is already the wide one, so the two name the same thing. This lives
// with its own record rather than with the rest of the large file names so
// that a translation unit which does not ask for it is left free to use the
// name itself, as <linux/dirent.h> does.
#if defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)
#define dirent64 dirent
#endif

#endif // LLVM_LIBC_MACROS_DIRENT_MACROS_H

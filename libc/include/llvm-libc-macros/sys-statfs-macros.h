//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Macros defined in sys/statfs.h header file.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SYS_STATFS_MACROS_H
#define LLVM_LIBC_MACROS_SYS_STATFS_MACROS_H

#ifdef __linux__
#include "linux/sys-statfs-macros.h"
#endif

// The name large file support gave struct statfs. LLVM-libc's ordinary
// record is already the wide one, so the two name the same thing. This lives
// with its own record rather than with the rest of the large file names so
// that a translation unit which does not ask for it is left free to use the
// name itself, as <asm/statfs.h> does.
#if defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)
#define statfs64 statfs
#endif

#endif // LLVM_LIBC_MACROS_SYS_STATFS_MACROS_H

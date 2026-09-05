//===-- Definition of macros from ftw.h -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_FTW_MACROS_H
#define LLVM_LIBC_MACROS_FTW_MACROS_H

// What the walk found, passed to the callback.
#define FTW_F 0   // A file which is not a directory.
#define FTW_D 1   // A directory, before its contents.
#define FTW_DNR 2 // A directory which could not be read.
#define FTW_NS 3  // Something which could not be stat'd.
#define FTW_SL 4  // A symbolic link, only under FTW_PHYS.
#define FTW_DP 5  // A directory, after its contents, only under FTW_DEPTH.
#define FTW_SLN 6 // A symbolic link whose target does not exist.

// How nftw walks.
#define FTW_PHYS 1  // Do not follow symbolic links.
#define FTW_MOUNT 2 // Stay on the file system the walk started on.
#define FTW_CHDIR 4 // Change directory into each one before reading it.
#define FTW_DEPTH 8 // Report a directory after its contents rather than before.
// The callback returns one of the FTW_CONTINUE values below rather than zero
// to carry on and non zero to stop.
#define FTW_ACTIONRETVAL 16

// What a callback may return under FTW_ACTIONRETVAL.
#define FTW_CONTINUE 0
#define FTW_STOP 1
#define FTW_SKIP_SUBTREE 2
#define FTW_SKIP_SIBLINGS 3

#endif // LLVM_LIBC_MACROS_FTW_MACROS_H

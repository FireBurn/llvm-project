//===-- Macros defined in sys/file.h header file --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SYS_FILE_MACROS_H
#define LLVM_LIBC_MACROS_SYS_FILE_MACROS_H

// The kind of lock flock is to place, and whether it should wait for it.
#define LOCK_SH 1 // A lock others may share.
#define LOCK_EX 2 // A lock only this holder may have.
#define LOCK_NB 4 // Report that the lock is taken rather than waiting.
#define LOCK_UN 8 // Release whichever lock is held.

#endif // LLVM_LIBC_MACROS_SYS_FILE_MACROS_H

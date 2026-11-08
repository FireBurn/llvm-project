//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Macros defined in spawn.h header file
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SPAWN_MACROS_H
#define LLVM_LIBC_MACROS_SPAWN_MACROS_H

// Flags for posix_spawnattr_setflags. Each says which piece of the parent's
// state the child is given afresh rather than inheriting.
#define POSIX_SPAWN_RESETIDS 0x01
#define POSIX_SPAWN_SETPGROUP 0x02
#define POSIX_SPAWN_SETSIGDEF 0x04
#define POSIX_SPAWN_SETSIGMASK 0x08
#define POSIX_SPAWN_SETSCHEDPARAM 0x10
#define POSIX_SPAWN_SETSCHEDULER 0x20
#define POSIX_SPAWN_USEVFORK 0x40
#define POSIX_SPAWN_SETSID 0x80
// Bionic's: mark every descriptor but the standard three close-on-exec before
// the file actions run.
#define POSIX_SPAWN_CLOEXEC_DEFAULT 0x100

#endif // LLVM_LIBC_MACROS_SPAWN_MACROS_H

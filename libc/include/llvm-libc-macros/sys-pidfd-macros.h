//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Macros defined in sys/pidfd.h header file.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SYS_PIDFD_MACROS_H
#define LLVM_LIBC_MACROS_SYS_PIDFD_MACROS_H

// The same as O_NONBLOCK and O_EXCL on every Linux target.
#define PIDFD_NONBLOCK 04000
#define PIDFD_THREAD 00200

#endif // LLVM_LIBC_MACROS_SYS_PIDFD_MACROS_H

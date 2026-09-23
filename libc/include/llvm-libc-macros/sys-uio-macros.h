//===-- Macros defined in sys/uio.h header file ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SYS_UIO_MACROS_H
#define LLVM_LIBC_MACROS_SYS_UIO_MACROS_H

// The most iovecs readv and writev take at once, the kernel's UIO_MAXIOV.
#define UIO_MAXIOV 1024

#endif // LLVM_LIBC_MACROS_SYS_UIO_MACROS_H

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Macros defined in sys/uio.h header file
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SYS_UIO_MACROS_H
#define LLVM_LIBC_MACROS_SYS_UIO_MACROS_H

// The most iovecs readv and writev take at once, the kernel's UIO_MAXIOV.
#define UIO_MAXIOV 1024

// Per call flags for preadv2, pwritev2 and the io_submit control block.
#define RWF_HIPRI 0x00000001
#define RWF_DSYNC 0x00000002
#define RWF_SYNC 0x00000004
#define RWF_NOWAIT 0x00000008
#define RWF_APPEND 0x00000010

#endif // LLVM_LIBC_MACROS_SYS_UIO_MACROS_H

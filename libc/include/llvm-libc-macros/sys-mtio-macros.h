//===-- Definitions from sys/mtio.h ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SYS_MTIO_MACROS_H
#define LLVM_LIBC_MACROS_SYS_MTIO_MACROS_H

// The operations, the structures they use and the requests that carry them
// are all the kernel's, and are the same whatever names a libc gives them, so
// they are taken from where they are defined rather than written out again.
// The kernel has since dropped the two configuration requests, which glibc
// and musl still define.
#include <linux/mtio.h>

#ifndef MTIOCGETCONFIG
#define MTIOCGETCONFIG _IOR('m', 4, struct mtconfiginfo)
#endif
#ifndef MTIOCSETCONFIG
#define MTIOCSETCONFIG _IOW('m', 5, struct mtconfiginfo)
#endif

#endif // LLVM_LIBC_MACROS_SYS_MTIO_MACROS_H

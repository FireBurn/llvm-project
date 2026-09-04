//===-- Macros defined in sys/eventfd.h header file -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_SYS_EVENTFD_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_SYS_EVENTFD_MACROS_H

#include "fcntl-macros.h"

// A read takes one from the count rather than all of it.
#define EFD_SEMAPHORE 1
#define EFD_CLOEXEC O_CLOEXEC
#define EFD_NONBLOCK O_NONBLOCK

#endif // LLVM_LIBC_MACROS_LINUX_SYS_EVENTFD_MACROS_H

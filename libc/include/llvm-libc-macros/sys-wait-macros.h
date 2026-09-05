//===-- Macros defined in sys/wait.h header file --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SYS_WAIT_MACROS_H
#define LLVM_LIBC_MACROS_SYS_WAIT_MACROS_H

// The signal numbers come with this header rather than only with <signal.h>:
// waiting on a child reports how a signal ended it, and callers name those
// signals without including anything further.
#include "signal-macros.h"

#ifdef __linux__
#include "linux/sys-wait-macros.h"
#endif

#endif // LLVM_LIBC_MACROS_SYS_WAIT_MACROS_H

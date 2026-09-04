//===-- Macros defined in sys/timerfd.h header file -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_SYS_TIMERFD_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_SYS_TIMERFD_MACROS_H

#include "fcntl-macros.h"

#define TFD_CLOEXEC O_CLOEXEC
#define TFD_NONBLOCK O_NONBLOCK

// The time given to timerfd_settime is a moment rather than a delay.
#define TFD_TIMER_ABSTIME 1
// Report it if the clock is set while an absolute timer is waiting on it.
#define TFD_TIMER_CANCEL_ON_SET 2

#endif // LLVM_LIBC_MACROS_LINUX_SYS_TIMERFD_MACROS_H

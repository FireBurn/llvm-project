//===-- Implementation header for timerfd_gettime ---------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_TIMERFD_TIMERFD_GETTIME_H
#define LLVM_LIBC_SRC_SYS_TIMERFD_TIMERFD_GETTIME_H

#include "hdr/types/clockid_t.h"
#include "hdr/types/struct_itimerspec.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int timerfd_gettime(int fd, struct itimerspec *curr_value);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_TIMERFD_TIMERFD_GETTIME_H

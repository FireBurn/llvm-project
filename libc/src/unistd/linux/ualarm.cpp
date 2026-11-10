//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of ualarm.
///
//===----------------------------------------------------------------------===//

#include "src/unistd/ualarm.h"
#include "hdr/types/struct_itimerval.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/setitimer.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(useconds_t, ualarm,
                   (useconds_t usecs, useconds_t interval)) {
  constexpr useconds_t USEC_PER_SEC = 1000000;
  struct itimerval timer;
  timer.it_value.tv_sec = usecs / USEC_PER_SEC;
  timer.it_value.tv_usec = usecs % USEC_PER_SEC;
  timer.it_interval.tv_sec = interval / USEC_PER_SEC;
  timer.it_interval.tv_usec = interval % USEC_PER_SEC;
  struct itimerval old;
  // ITIMER_REAL is 0.
  if (!linux_syscalls::setitimer(0, &timer, &old))
    return 0;
  return static_cast<useconds_t>(old.it_value.tv_sec * USEC_PER_SEC +
                                 old.it_value.tv_usec);
}

} // namespace LIBC_NAMESPACE_DECL

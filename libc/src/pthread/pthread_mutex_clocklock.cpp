//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of pthread_mutex_clocklock
///
//===----------------------------------------------------------------------===//

#include "pthread_mutex_clocklock.h"

#include "hdr/errno_macros.h"
#include "hdr/time_macros.h"
#include "hdr/types/clockid_t.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/pthread/pthread_mutex_timed_lock_utils.h"

#include <pthread.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_mutex_clocklock,
                   (pthread_mutex_t *__restrict mutex, clockid_t clockid,
                    const struct timespec *__restrict abstime)) {
  if (clockid != CLOCK_MONOTONIC && clockid != CLOCK_REALTIME)
    return EINVAL;
  return pthread_mutex_utils::timed_lock(mutex, abstime,
                                         clockid == CLOCK_REALTIME);
}

} // namespace LIBC_NAMESPACE_DECL

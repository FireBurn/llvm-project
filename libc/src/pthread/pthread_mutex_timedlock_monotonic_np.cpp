//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of pthread_mutex_timedlock_monotonic_np
///
//===----------------------------------------------------------------------===//

#include "pthread_mutex_timedlock_monotonic_np.h"

#include "hdr/errno_macros.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/pthread/pthread_mutex_timed_lock_utils.h"

#include <pthread.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_mutex_timedlock_monotonic_np,
                   (pthread_mutex_t *__restrict mutex,
                    const struct timespec *__restrict abstime)) {
  return pthread_mutex_utils::timed_lock(mutex, abstime, /*is_realtime=*/false);
}

} // namespace LIBC_NAMESPACE_DECL

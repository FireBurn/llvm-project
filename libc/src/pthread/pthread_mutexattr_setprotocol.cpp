//===-- Implementation of pthread_mutexattr_setprotocol -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "pthread_mutexattr_setprotocol.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include <pthread.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_mutexattr_setprotocol,
                   (pthread_mutexattr_t * attr, int protocol)) {
  if (attr == nullptr)
    return EINVAL;
  switch (protocol) {
  case PTHREAD_PRIO_NONE:
    return 0;
  case PTHREAD_PRIO_INHERIT:
  case PTHREAD_PRIO_PROTECT:
    // Nothing here raises the priority of a lock holder: the mutex does not
    // take the kernel's priority-inheriting futex. Saying so is better than
    // taking the request and leaving a caller to find out under load.
    return ENOTSUP;
  default:
    return EINVAL;
  }
}

} // namespace LIBC_NAMESPACE_DECL

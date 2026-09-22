//===-- Implementation of pthread_attr_setschedpolicy ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "pthread_attr_setschedpolicy.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include <pthread.h>
#include <sched.h>

namespace LIBC_NAMESPACE_DECL {

// The policy a thread created with these attributes is given, when it is
// created with PTHREAD_EXPLICIT_SCHED. The kernel decides whether the
// process may ask for it, so that is reported at creation rather than here.
LLVM_LIBC_FUNCTION(int, pthread_attr_setschedpolicy,
                   (pthread_attr_t * attr, int policy)) {
  if (attr == nullptr)
    return EINVAL;
  switch (policy) {
  case SCHED_OTHER:
  case SCHED_FIFO:
  case SCHED_RR:
    attr->__schedpolicy = policy;
    return 0;
  default:
    return EINVAL;
  }
}

} // namespace LIBC_NAMESPACE_DECL

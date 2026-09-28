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
#include "src/pthread/pthread_mutexattr.h"

#include <pthread.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_mutexattr_setprotocol,
                   (pthread_mutexattr_t * attr, int protocol)) {
  if (attr == nullptr)
    return EINVAL;
  switch (protocol) {
  case PTHREAD_PRIO_NONE:
    break;
  case PTHREAD_PRIO_INHERIT:
#if defined(__linux__)
    break;
#else
    return ENOTSUP;
#endif
  case PTHREAD_PRIO_PROTECT:
    // Nothing here raises a lock holder to a ceiling. Saying so is better
    // than taking the request and leaving a caller to find out under load.
    return ENOTSUP;
  default:
    return EINVAL;
  }
  pthread_mutexattr_t old = *attr;
  old &= ~unsigned(PThreadMutexAttrPos::PROTOCOL_MASK);
  *attr = old | (protocol << unsigned(PThreadMutexAttrPos::PROTOCOL_SHIFT));
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

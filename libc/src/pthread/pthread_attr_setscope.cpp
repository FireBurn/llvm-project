//===-- Implementation of pthread_attr_setscope ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "pthread_attr_setscope.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include <pthread.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_attr_setscope,
                   (pthread_attr_t * attr, int scope)) {
  if (attr == nullptr)
    return EINVAL;
  // A thread on Linux is scheduled against every other thread on the
  // machine, so the system scope is the only one there is. Asking for the
  // other is a request the kernel cannot meet rather than a mistake.
  switch (scope) {
  case PTHREAD_SCOPE_SYSTEM:
    return 0;
  case PTHREAD_SCOPE_PROCESS:
    return ENOTSUP;
  default:
    return EINVAL;
  }
}

} // namespace LIBC_NAMESPACE_DECL

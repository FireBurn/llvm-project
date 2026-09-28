//===-- Implementation of pthread_mutexattr_getprotocol -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "pthread_mutexattr_getprotocol.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/pthread/pthread_mutexattr.h"

#include <pthread.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_mutexattr_getprotocol,
                   (const pthread_mutexattr_t *attr, int *protocol)) {
  if (attr == nullptr || protocol == nullptr)
    return EINVAL;
  *protocol = get_mutexattr_protocol(*attr);
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

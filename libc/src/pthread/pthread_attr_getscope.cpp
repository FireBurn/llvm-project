//===-- Implementation of pthread_attr_getscope ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "pthread_attr_getscope.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include <pthread.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_attr_getscope,
                   (const pthread_attr_t *attr, int *scope)) {
  if (attr == nullptr || scope == nullptr)
    return EINVAL;
  // The only scope a thread here is ever given.
  *scope = PTHREAD_SCOPE_SYSTEM;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

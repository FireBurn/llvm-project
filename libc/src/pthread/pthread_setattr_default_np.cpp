//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of pthread_setattr_default_np
///
//===----------------------------------------------------------------------===//

#include "pthread_setattr_default_np.h"

#include "hdr/errno_macros.h"
#include "hdr/pthread_macros.h"
#include "hdr/types/pthread_attr_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/pthread/pthread_attr.h"
#include "src/pthread/pthread_default_attr.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_setattr_default_np,
                   (const pthread_attr_t *attr)) {
  if (attr == nullptr)
    return EINVAL;
  if (attr->__stacksize && attr->__stacksize < PTHREAD_STACK_MIN)
    return EINVAL;
  if (attr->__guardsize % EXEC_PAGESIZE != 0)
    return EINVAL;
  if (attr->__detachstate != PTHREAD_CREATE_DETACHED &&
      attr->__detachstate != PTHREAD_CREATE_JOINABLE)
    return EINVAL;
  // A stack of the caller's own belongs to one thread, not to all of them.
  pthread_attr_t copy = *attr;
  copy.__stack = nullptr;
  set_default_thread_attr(copy);
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

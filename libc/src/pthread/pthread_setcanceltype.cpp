//===-- Implementation of pthread_setcanceltype ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_setcanceltype.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/cancel.h"
#include "src/__support/threads/thread.h"

#include <pthread.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_setcanceltype, (int type, int *oldtype)) {
  if (type != PTHREAD_CANCEL_DEFERRED && type != PTHREAD_CANCEL_ASYNCHRONOUS)
    return EINVAL;

  ThreadAttributes *attrib = internal::self.attrib;
  if (attrib == nullptr)
    return EINVAL;

  if (oldtype != nullptr)
    *oldtype = static_cast<int>(attrib->cancel_type);
  attrib->cancel_type = static_cast<uint32_t>(type);

  if (type == PTHREAD_CANCEL_ASYNCHRONOUS)
    internal::cancel_point();
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

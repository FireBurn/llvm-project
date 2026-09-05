//===-- Implementation of pthread_setcancelstate --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_setcancelstate.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/cancel.h"
#include "src/__support/threads/thread.h"

#include <pthread.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_setcancelstate, (int state, int *oldstate)) {
  if (state != PTHREAD_CANCEL_ENABLE && state != PTHREAD_CANCEL_DISABLE)
    return EINVAL;

  ThreadAttributes *attrib = internal::self.attrib;
  if (attrib == nullptr)
    return EINVAL;

  const uint32_t previous =
      attrib->cancel_state.exchange(static_cast<uint32_t>(state));
  if (oldstate != nullptr)
    *oldstate = static_cast<int>(previous);

  // Turning cancellation back on is not itself a cancellation point, so a
  // request that arrived while it was off waits for the next one. A thread
  // asking to stop at once is the exception: it asked for no wait.
  if (state == PTHREAD_CANCEL_ENABLE &&
      attrib->cancel_type == uint32_t(CancelType::ASYNCHRONOUS))
    internal::cancel_point();
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

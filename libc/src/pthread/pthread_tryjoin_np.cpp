//===-- Implementation of the pthread_tryjoin_np function -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "pthread_tryjoin_np.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/thread.h"

#include <pthread.h> // For pthread_* type definitions.

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_tryjoin_np, (pthread_t th, void **retval)) {
  auto *thread = reinterpret_cast<Thread *>(&th);
  return thread->try_join(retval);
}

} // namespace LIBC_NAMESPACE_DECL

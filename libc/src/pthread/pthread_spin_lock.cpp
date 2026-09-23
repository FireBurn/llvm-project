//===-- Implementation of pthread_spin_lock function ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_spin_lock.h"
#include "hdr/errno_macros.h"
#include "src/__support/CPP/atomic.h"
#include "src/__support/common.h"
#include "src/__support/threads/identifier.h"
#include "src/__support/threads/sleep.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_spin_lock, (pthread_spinlock_t * lock)) {
  if (!lock)
    return EINVAL;
  cpp::AtomicRef<int> word(*lock);
  int self = static_cast<int>(internal::gettid());
  for (;;) {
    int holder = 0;
    if (word.compare_exchange_strong(holder, self, cpp::MemoryOrder::ACQUIRE,
                                     cpp::MemoryOrder::RELAXED))
      return 0;
    if (holder == self)
      return EDEADLK;
    if (holder == -1)
      return EINVAL;
    while (word.load(cpp::MemoryOrder::RELAXED) > 0)
      sleep_briefly();
  }
}

} // namespace LIBC_NAMESPACE_DECL

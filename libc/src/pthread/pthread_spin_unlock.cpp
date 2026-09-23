//===-- Implementation of pthread_spin_unlock function --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_spin_unlock.h"
#include "hdr/errno_macros.h"
#include "src/__support/CPP/atomic.h"
#include "src/__support/common.h"
#include "src/__support/threads/identifier.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_spin_unlock, (pthread_spinlock_t * lock)) {
  if (!lock)
    return EINVAL;
  cpp::AtomicRef<int> word(*lock);
  int holder = word.load(cpp::MemoryOrder::RELAXED);
  if (holder == -1)
    return EINVAL;
  if (holder != static_cast<int>(internal::gettid()))
    return EPERM;
  word.store(0, cpp::MemoryOrder::RELEASE);
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

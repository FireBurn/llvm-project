//===-- Implementation of pthread_spin_destroy function -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_spin_destroy.h"
#include "hdr/errno_macros.h"
#include "src/__support/CPP/atomic.h"
#include "src/__support/common.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_spin_destroy, (pthread_spinlock_t * lock)) {
  if (!lock)
    return EINVAL;
  int holder = 0;
  if (cpp::AtomicRef<int>(*lock).compare_exchange_strong(
          holder, -1, cpp::MemoryOrder::RELAXED, cpp::MemoryOrder::RELAXED))
    return 0;
  return holder == -1 ? EINVAL : EBUSY;
}

} // namespace LIBC_NAMESPACE_DECL

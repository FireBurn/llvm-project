//===-- Implementation of pthread_spin_init function ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_spin_init.h"
#include "hdr/errno_macros.h"
#include "hdr/pthread_macros.h"
#include "src/__support/CPP/atomic.h"
#include "src/__support/common.h"

namespace LIBC_NAMESPACE_DECL {

// A spin lock is the thread id of its holder, zero when free, or -1 once
// destroyed. It never sleeps, so being shared between processes needs
// nothing more.
LLVM_LIBC_FUNCTION(int, pthread_spin_init,
                   (pthread_spinlock_t * lock, int pshared)) {
  if (!lock)
    return EINVAL;
  if (pshared != PTHREAD_PROCESS_SHARED && pshared != PTHREAD_PROCESS_PRIVATE)
    return EINVAL;
  cpp::AtomicRef<int>(*lock).store(0, cpp::MemoryOrder::RELEASE);
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

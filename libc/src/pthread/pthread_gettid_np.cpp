//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of pthread_gettid_np.
///
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_gettid_np.h"
#include "hdr/types/pid_t.h"
#include "hdr/types/pthread_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/__support/threads/thread.h"

namespace LIBC_NAMESPACE_DECL {

// The kernel's id of the thread, which gettid gives for the calling one.
LLVM_LIBC_FUNCTION(pid_t, pthread_gettid_np, (pthread_t th)) {
  auto *thread = reinterpret_cast<Thread *>(&th);
  LIBC_CRASH_ON_NULLPTR(thread->attrib);
  return thread->attrib->tid;
}

} // namespace LIBC_NAMESPACE_DECL

//===-- Implementation of setlogmask --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/syslog/setlogmask.h"

#include "src/__support/CPP/mutex.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/syslog/syslog_state.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, setlogmask, (int mask)) {
  cpp::lock_guard guard(syslog_internal::log_mutex);
  int old = syslog_internal::log_state.mask;
  // A mask of zero is a request for the current one rather than for silence.
  if (mask != 0)
    syslog_internal::log_state.mask = mask;
  return old;
}

} // namespace LIBC_NAMESPACE_DECL

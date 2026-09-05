//===-- Implementation of closelog ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/syslog/closelog.h"

#include "hdr/syslog_macros.h"
#include "src/__support/CPP/mutex.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/syslog/syslog_state.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, closelog, (void)) {
  cpp::lock_guard guard(syslog_internal::log_mutex);
  syslog_internal::disconnect_unlocked();

  syslog_internal::State &s = syslog_internal::log_state;
  s.ident = nullptr;
  s.option = 0;
  s.facility = LOG_USER;
}

} // namespace LIBC_NAMESPACE_DECL

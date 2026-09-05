//===-- Implementation of openlog -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/syslog/openlog.h"

#include "hdr/syslog_macros.h"
#include "src/__support/CPP/mutex.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/syslog/syslog_state.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, openlog,
                   (const char *ident, int option, int facility)) {
  cpp::lock_guard guard(syslog_internal::log_mutex);
  syslog_internal::State &s = syslog_internal::log_state;

  // The string is not copied. The caller has to keep it around, which is
  // what every other implementation asks of it too.
  s.ident = ident;
  s.option = option;
  if (facility != 0)
    s.facility = facility;

  if (option & LOG_NDELAY)
    syslog_internal::connect_unlocked();
}

} // namespace LIBC_NAMESPACE_DECL

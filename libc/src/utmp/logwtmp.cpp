//===-- Implementation of logwtmp -------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/utmp/logwtmp.h"

#include "hdr/types/struct_utmp.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/utmp/updwtmp.h"
#include "src/utmp/utmp_fill.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// Appends one record to the log of logins and logouts. An empty name is a
// logout, which is how the callers of this have always said so.
LLVM_LIBC_FUNCTION(void, logwtmp,
                   (const char *line, const char *name, const char *host)) {
  struct utmp record = {};
  record.ut_type =
      (name != nullptr && name[0] != '\0') ? USER_PROCESS : DEAD_PROCESS;
  record.ut_pid =
      static_cast<pid_t>(LIBC_NAMESPACE::syscall_impl<long>(SYS_getpid));
  utmp_fill::field(record.ut_line, sizeof(record.ut_line), line);
  utmp_fill::field(record.ut_user, sizeof(record.ut_user), name);
  utmp_fill::field(record.ut_host, sizeof(record.ut_host), host);
  utmp_fill::stamp(&record);
  LIBC_NAMESPACE::updwtmp(_PATH_WTMP, &record);
}

} // namespace LIBC_NAMESPACE_DECL

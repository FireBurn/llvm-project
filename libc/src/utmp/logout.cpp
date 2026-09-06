//===-- Implementation of logout --------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/utmp/logout.h"

#include "hdr/types/struct_utmp.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/utmp/endutent.h"
#include "src/utmp/getutline.h"
#include "src/utmp/pututline.h"
#include "src/utmp/setutent.h"
#include "src/utmp/utmp_fill.h"

namespace LIBC_NAMESPACE_DECL {

// Marks the record for a terminal as belonging to a process which has ended,
// leaving the line behind so that the entry can be found again. Returns
// non-zero if there was a record to mark.
LLVM_LIBC_FUNCTION(int, logout, (const char *line)) {
  if (line == nullptr)
    return 0;
  struct utmp key = {};
  utmp_fill::field(key.ut_line, sizeof(key.ut_line), line);

  LIBC_NAMESPACE::setutent();
  struct utmp *found = LIBC_NAMESPACE::getutline(&key);
  if (found == nullptr) {
    LIBC_NAMESPACE::endutent();
    return 0;
  }

  struct utmp record;
  inline_memcpy(&record, found, sizeof(record));
  record.ut_type = DEAD_PROCESS;
  utmp_fill::field(record.ut_user, sizeof(record.ut_user), nullptr);
  utmp_fill::field(record.ut_host, sizeof(record.ut_host), nullptr);
  utmp_fill::stamp(&record);
  const bool written = LIBC_NAMESPACE::pututline(&record) != nullptr;
  LIBC_NAMESPACE::endutent();
  return written ? 1 : 0;
}

} // namespace LIBC_NAMESPACE_DECL

//===-- Implementation of timegm ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/time/timegm.h"

#include "hdr/types/struct_tm.h"
#include "hdr/types/time_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/time/time_constants.h"
#include "src/time/time_utils.h"

namespace LIBC_NAMESPACE_DECL {

// What mktime would give if the fields were read as UTC, which is what this
// is for: mktime reads them as local time.
LLVM_LIBC_FUNCTION(time_t, timegm, (struct tm * tm)) {
  LIBC_CRASH_ON_NULLPTR(tm);

  auto seconds = time_utils::mktime_internal(tm);
  if (!seconds) {
    libc_errno = time_utils::TIME_OVERFLOW;
    return time_constants::OUT_OF_RANGE_RETURN_VALUE;
  }

  // The fields are put back in range, the same as mktime does.
  auto status = time_utils::update_from_seconds(*seconds, tm);
  if (!status) {
    libc_errno = status.error();
    return time_constants::OUT_OF_RANGE_RETURN_VALUE;
  }

  return *seconds;
}

} // namespace LIBC_NAMESPACE_DECL

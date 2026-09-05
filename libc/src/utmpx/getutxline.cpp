//===-- Implementation of getutxline --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/utmpx/getutxline.h"

#include "hdr/types/struct_utmpx.h"
#include "hdr/utmpx_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/utmpx/utmpx_db.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// The field is not required to be terminated when it fills the array, so
// the comparison stops at the end of it either way.
bool same_line(const char *a, const char *b) {
  for (int i = 0; i < __UT_LINESIZE; ++i) {
    if (a[i] != b[i])
      return false;
    if (a[i] == '\0')
      return true;
  }
  return true;
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(struct utmpx *, getutxline, (const struct utmpx *line)) {
  if (line == nullptr)
    return nullptr;
  while (utmpx_db::read_next()) {
    const struct utmpx &e = utmpx_db::entry;
    if ((e.ut_type == LOGIN_PROCESS || e.ut_type == USER_PROCESS) &&
        same_line(e.ut_line, line->ut_line))
      return &utmpx_db::entry;
  }
  return nullptr;
}

} // namespace LIBC_NAMESPACE_DECL

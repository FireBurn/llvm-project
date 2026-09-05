//===-- Implementation of getutxid ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/utmpx/getutxid.h"

#include "hdr/types/struct_utmpx.h"
#include "hdr/utmpx_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/utmpx/utmpx_db.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

bool same_id(const char *a, const char *b) {
  for (int i = 0; i < 4; ++i)
    if (a[i] != b[i])
      return false;
  return true;
}

// Whether the record is the one being looked for. A search for one of the
// four kinds of clock or run level record matches on the type alone;
// anything else matches on the inittab entry.
bool matches(const struct utmpx *entry, const struct utmpx *id) {
  switch (id->ut_type) {
  case RUN_LVL:
  case BOOT_TIME:
  case NEW_TIME:
  case OLD_TIME:
    return entry->ut_type == id->ut_type;
  case INIT_PROCESS:
  case LOGIN_PROCESS:
  case USER_PROCESS:
  case DEAD_PROCESS:
    switch (entry->ut_type) {
    case INIT_PROCESS:
    case LOGIN_PROCESS:
    case USER_PROCESS:
    case DEAD_PROCESS:
      return same_id(entry->ut_id, id->ut_id);
    default:
      return false;
    }
  default:
    return false;
  }
}

} // anonymous namespace

// The search carries on from where the iteration is, rather than starting
// over, which is what lets a caller walk every record with the same id.
LLVM_LIBC_FUNCTION(struct utmpx *, getutxid, (const struct utmpx *id)) {
  if (id == nullptr)
    return nullptr;
  while (utmpx_db::read_next())
    if (matches(&utmpx_db::entry, id))
      return &utmpx_db::entry;
  return nullptr;
}

} // namespace LIBC_NAMESPACE_DECL

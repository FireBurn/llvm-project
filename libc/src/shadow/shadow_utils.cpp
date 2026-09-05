//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Helper functions for shadow.
///
//===----------------------------------------------------------------------===//

#include "src/shadow/shadow_utils.h"
#include "hdr/types/struct_spwd.h"
#include "src/__support/flat_file_db.h"
#include "src/__support/macros/attributes.h"

#ifndef LIBC_COPT_SHADOW_FILE_PATH
#define LIBC_COPT_SHADOW_FILE_PATH "/etc/shadow"
#endif

namespace LIBC_NAMESPACE_DECL {
namespace shadow_db {

static LIBC_CONSTINIT internal::FlatFileDatabase<struct spwd>
    db(LIBC_COPT_SHADOW_FILE_PATH);
// These are process wide and are not under a lock: getspent is not
// reentrant, and the _r calls open the file for themselves.
static char line_buffer[shadow::SHADOW_LINE_SIZE];
static struct spwd shadow_entry;

void TESTONLY_set_shadow_path(const char *path) { db.set_path(path); }

const char *path() { return db.path(); }

ErrorOr<void> open() { return db.setdb(); }

ErrorOr<void> close() { return db.enddb(); }

ErrorOr<struct spwd *> read_next() {
  auto res = db.getnext(&shadow_entry, line_buffer);
  if (!res.has_value())
    return Error(res.error());
  if (!res.value())
    return nullptr;
  return &shadow_entry;
}

} // namespace shadow_db
} // namespace LIBC_NAMESPACE_DECL

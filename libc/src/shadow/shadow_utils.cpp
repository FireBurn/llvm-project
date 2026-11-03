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
#include "src/__support/macros/attributes.h"
#include "src/__support/nss/dispatch.h"
#include "src/__support/nss/lookup.h"
#include "src/__support/pwd/dynamic_buffer.h"
#include "src/__support/pwd/flat_file_db.h"

#ifndef LIBC_COPT_SHADOW_FILE_PATH
#define LIBC_COPT_SHADOW_FILE_PATH "/etc/shadow"
#endif

namespace LIBC_NAMESPACE_DECL {
namespace shadow_db {

static LIBC_CONSTINIT pwd::FlatFileDatabase<struct spwd>
    db(LIBC_COPT_SHADOW_FILE_PATH);
// These are process wide and are not under a lock: getspent is not
// reentrant, and the _r calls open the file for themselves.
static LIBC_CONSTINIT pwd::DynamicBuffer line_buffer;
static struct spwd shadow_entry;
static nss::Cursor cursor;

void TESTONLY_set_shadow_path(const char *path) { db.set_path(path); }

const char *path() { return db.path(); }

ErrorOr<void> open() {
  nss::reset(cursor, "endspent");
  return db.setdb();
}

ErrorOr<void> close() {
  nss::reset(cursor, "endspent");
  return db.enddb();
}

ErrorOr<bool> read_next(struct spwd *entry, cpp::span<char> buffer) {
  return nss::next(cursor, nss::SHADOW, "setspent", "getspent_r", "endspent",
                   entry, buffer, [&] { return db.getnext(entry, buffer); });
}

ErrorOr<struct spwd *> read_next() {
  auto res = nss::with_growing_buffer(line_buffer, [](cpp::span<char> buffer) {
    return read_next(&shadow_entry, buffer);
  });
  if (!res.has_value())
    return Error(res.error());
  if (!res.value())
    return nullptr;
  return &shadow_entry;
}

} // namespace shadow_db
} // namespace LIBC_NAMESPACE_DECL

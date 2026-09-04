//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Helper functions for grp.
///
//===----------------------------------------------------------------------===//

#include "src/grp/grp_utils.h"
#include "hdr/errno_macros.h"
#include "hdr/types/struct_group.h"
#include "src/__support/CPP/span.h"
#include "src/__support/flat_file_db.h"
#include "src/__support/macros/attributes.h"
#include "src/string/string_utils.h"

#ifndef LIBC_COPT_GRP_FILE_PATH
#define LIBC_COPT_GRP_FILE_PATH "/etc/group"
#endif

namespace LIBC_NAMESPACE_DECL {
namespace grp {

char *member_list[GROUP_MEMBER_MAX];

ErrorOr<struct group> parse_group_line(char *line) {
  if (!line)
    return Error(EINVAL);

  struct group grp;
  size_t len = internal::string_length(line);
  if (!internal::parse_line(cpp::span<char>(line, len + 1), &grp))
    return Error(EINVAL);

  return grp;
}

} // namespace grp

namespace group {

static LIBC_CONSTINIT internal::FlatFileDatabase<struct group>
    db(LIBC_COPT_GRP_FILE_PATH);
// Note: These static buffers are process-global and NOT protected by a mutex
// at this stage. POSIX getgrent is non-reentrant.
static char line_buffer[grp::GROUP_LINE_SIZE];
static struct group group_entry;

void TESTONLY_set_group_path(const char *path) { db.set_path(path); }

const char *path() { return db.path(); }

ErrorOr<void> open() { return db.setdb(); }

ErrorOr<void> close() { return db.enddb(); }

ErrorOr<struct group *> read_next() {
  auto res = db.getnext(&group_entry, line_buffer);
  if (!res.has_value())
    return Error(res.error());
  if (!res.value())
    return nullptr;
  return &group_entry;
}

} // namespace group
} // namespace LIBC_NAMESPACE_DECL

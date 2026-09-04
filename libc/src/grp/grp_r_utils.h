//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The search getgrnam_r and getgrgid_r share.
///
/// Neither may touch anything another thread might be using, so each opens
/// the file for itself and parses into the caller's buffer. The list of
/// member names needs an array of pointers as well as the names themselves,
/// and that is taken from the end of the same buffer.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_GRP_GRP_R_UTILS_H
#define LLVM_LIBC_SRC_GRP_GRP_R_UTILS_H

#include "hdr/errno_macros.h"
#include "hdr/stdint_proxy.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_group.h"
#include "src/__support/CPP/span.h"
#include "src/__support/flat_file_db.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/grp/grp_utils.h"

namespace LIBC_NAMESPACE_DECL {
namespace group_r {

// How much of the caller's buffer goes to the member pointers. A line can
// hold no more members than it has room for one character and one separator
// each, but a whole buffer's worth of pointers would leave nothing for the
// line, so this takes a quarter of it.
constexpr size_t MEMBER_SHARE = 4;

template <typename Match>
LIBC_INLINE int find(struct group *entry, char *buffer, size_t buflen,
                     struct group **result, Match matches) {
  if (result != nullptr)
    *result = nullptr;
  if (entry == nullptr || buffer == nullptr || result == nullptr)
    return EINVAL;

  // Carve the member pointers off the end, aligned for a pointer.
  size_t pointer_bytes = buflen / MEMBER_SHARE;
  pointer_bytes -= pointer_bytes % sizeof(char *);
  if (pointer_bytes < sizeof(char *) * 2 || buflen - pointer_bytes < 2)
    return ERANGE;
  size_t line_bytes = buflen - pointer_bytes;
  char **members = reinterpret_cast<char **>(
      reinterpret_cast<uintptr_t>(buffer + line_bytes) &
      ~(alignof(char *) - 1));
  size_t max_members = pointer_bytes / sizeof(char *);

  internal::FlatFileDatabase<struct group> db(group::path());
  if (auto opened = db.setdb(); !opened.has_value())
    return opened.error();

  for (;;) {
    // The engine's parse uses the shared member array, so the line is read
    // here and parsed with the caller's array instead.
    auto read = db.getline(cpp::span<char>(buffer, line_bytes));
    if (!read.has_value()) {
      db.enddb();
      return read.error();
    }
    if (read.value() == 0)
      break; // The end of the file, and no match is not an error.
    if (!grp::parse_line_into(cpp::span<char>(buffer, read.value() + 1), entry,
                              members, max_members))
      continue; // A line which is not an entry is skipped, not an error.
    if (matches(entry)) {
      db.enddb();
      *result = entry;
      return 0;
    }
  }
  db.enddb();
  return 0;
}

} // namespace group_r
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_GRP_GRP_R_UTILS_H

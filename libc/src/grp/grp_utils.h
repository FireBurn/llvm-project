//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Declarations of helper functions and parser for grp.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_GRP_GRP_UTILS_H
#define LLVM_LIBC_SRC_GRP_GRP_UTILS_H

#include "hdr/errno_macros.h"
#include "hdr/types/gid_t.h"
#include "hdr/types/struct_group.h"
#include "src/__support/CPP/span.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/error_or.h"
#include "src/__support/field_tokenizer.h"
#include "src/__support/flat_file_db.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/str_to_integer.h"
#include "src/string/string_utils.h"

namespace LIBC_NAMESPACE_DECL {
namespace grp {

// The member list of one group entry, split out of the line in place.
// A line can hold no more members than it has room for one character and
// one separator each.
constexpr size_t GROUP_LINE_SIZE = 1024;
constexpr size_t GROUP_MEMBER_MAX = GROUP_LINE_SIZE / 2 + 1;

// Where the member pointers for the entry being parsed are put. The parser
// has nowhere else to put them, since struct group only holds the array.
extern char *member_list[GROUP_MEMBER_MAX];

} // namespace grp

namespace internal {

// Parses a colon-separated line in-place into a struct group.
template <>
LIBC_INLINE bool parse_line<struct group>(cpp::span<char> line,
                                          struct group *grp) {
  if (line.empty() || !grp)
    return false;

  FieldTokenizer tokenizer(line);

  auto name = tokenizer.next_field();
  if (!name)
    return false;
  grp->gr_name = name->data();

  auto passwd = tokenizer.next_field();
  if (!passwd)
    return false;
  grp->gr_passwd = passwd->data();

  auto gid_str = tokenizer.next_field();
  if (!gid_str || gid_str->empty() || !isdigit(gid_str->front()))
    return false;
  auto gid_res = strtointeger<gid_t>(gid_str->data(), 10);
  if (gid_res.has_error() || gid_res.parsed_len <= 0 ||
      static_cast<size_t>(gid_res.parsed_len) >= gid_str->size() ||
      (*gid_str)[gid_res.parsed_len] != '\0')
    return false;
  grp->gr_gid = gid_res.value;

  // The rest of the line is the member list, which is comma separated
  // rather than colon separated and may be empty.
  auto members = tokenizer.next_field();
  if (!members)
    return false;

  size_t count = 0;
  char *p = members->data();
  if (*p != '\0') {
    FieldTokenizer member_tokenizer(*members, ',');
    while (auto member = member_tokenizer.next_field()) {
      if (count + 1 >= grp::GROUP_MEMBER_MAX)
        return false;
      grp::member_list[count++] = member->data();
    }
  }
  grp::member_list[count] = nullptr;
  grp->gr_mem = grp::member_list;

  return true;
}

} // namespace internal

namespace grp {

// Parses a colon-separated group database line into a struct group.
ErrorOr<struct group> parse_group_line(char *line);

} // namespace grp

namespace group {

// Overrides the default group file path for testing purposes.
void TESTONLY_set_group_path(const char *path);

// Opens or rewinds the group file.
ErrorOr<void> open();

// Closes the group file.
ErrorOr<void> close();

// Reads the next entry from the group database.
ErrorOr<struct group *> read_next();

} // namespace group
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_GRP_GRP_UTILS_H

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

namespace grp {

// Parses a colon-separated line in place into |grp|, putting the pointers to
// the member names in |members|, which has room for |max_members| of them
// counting the null which ends the list. A caller which must be reentrant
// supplies its own array; the shared one is used otherwise.
LIBC_INLINE bool parse_line_into(cpp::span<char> line, struct group *grp,
                                 char **members, size_t max_members) {
  if (line.empty() || !grp || !members || max_members < 1)
    return false;

  internal::FieldTokenizer tokenizer(line);

  auto name = tokenizer.next_field();
  if (!name)
    return false;
  grp->gr_name = name->data();

  auto passwd = tokenizer.next_field();
  if (!passwd)
    return false;
  grp->gr_passwd = passwd->data();

  // A name opening with a plus or a minus is one of the lines that used to
  // pull entries in from the network database. Those carry no number of their
  // own, and are read as zero rather than turned away.
  const bool from_network_database =
      grp->gr_name[0] == '+' || grp->gr_name[0] == '-';

  auto gid_str = tokenizer.next_field();
  if (!gid_str)
    return false;
  if (gid_str->front() == '\0') {
    if (!from_network_database)
      return false;
    grp->gr_gid = 0;
  } else {
    if (!internal::isdigit(gid_str->front()))
      return false;
    auto gid_res = internal::strtointeger<gid_t>(gid_str->data(), 10);
    if (gid_res.has_error() || gid_res.parsed_len <= 0 ||
        static_cast<size_t>(gid_res.parsed_len) >= gid_str->size() ||
        (*gid_str)[gid_res.parsed_len] != '\0')
      return false;
    grp->gr_gid = gid_res.value;
  }

  // The rest of the line is the member list, which is comma separated rather
  // than colon separated. It may be empty, and may be missing altogether:
  // a line that stops after the number names a group with nobody in it.
  auto member_field = tokenizer.next_field();

  size_t count = 0;
  if (member_field && member_field->front() != '\0') {
    internal::FieldTokenizer member_tokenizer(*member_field, ',');
    while (auto member = member_tokenizer.next_field()) {
      if (count + 1 >= max_members)
        return false;
      members[count++] = member->data();
    }
  }
  members[count] = nullptr;
  grp->gr_mem = members;

  return true;
}

// Parses a colon-separated group database line into a struct group.
ErrorOr<struct group> parse_group_line(char *line);

} // namespace grp

namespace internal {

// The parse the flat file engine calls, which uses the shared array and so
// is not reentrant. getgrnam_r and getgrgid_r go through parse_line_into
// with an array of their own instead.
template <>
LIBC_INLINE bool parse_line<struct group>(cpp::span<char> line,
                                          struct group *grp) {
  return grp::parse_line_into(line, grp, grp::member_list,
                              grp::GROUP_MEMBER_MAX);
}

} // namespace internal

namespace group {

// Overrides the default group file path for testing purposes.
void TESTONLY_set_group_path(const char *path);

// Which file the lookups read. The reentrant ones open it for themselves
// rather than sharing the handle the iteration uses.
const char *path();

// Opens or rewinds the group file.
ErrorOr<void> open();

// Closes the group file.
ErrorOr<void> close();

// Reads the next entry from the group database.
ErrorOr<struct group *> read_next();

} // namespace group
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_GRP_GRP_UTILS_H

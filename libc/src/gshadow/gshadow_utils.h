//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Declarations of helper functions and parser for gshadow.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_GSHADOW_GSHADOW_UTILS_H
#define LLVM_LIBC_SRC_GSHADOW_GSHADOW_UTILS_H

#include "hdr/errno_macros.h"
#include "hdr/types/struct_sgrp.h"
#include "src/__support/CPP/span.h"
#include "src/__support/error_or.h"
#include "src/__support/field_tokenizer.h"
#include "src/__support/flat_file_db.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace gshadow {

// A line can hold no more names than it has room for one character and one
// separator each, and the two lists share the line.
constexpr size_t GSHADOW_LINE_SIZE = 1024;
constexpr size_t GSHADOW_NAME_MAX = GSHADOW_LINE_SIZE / 2 + 1;

// Where the pointers for the entry being parsed go. The struct only holds
// the arrays, so the parser has nowhere else to put them.
extern char *admin_list[GSHADOW_NAME_MAX];
extern char *member_list[GSHADOW_NAME_MAX];

// Splits a comma separated field in place into |out|, which has room for
// |max| pointers counting the null which ends the list.
bool split_names(cpp::span<char> field, char **out, size_t max);

// Parses a colon separated line in place into |entry|, putting the two name
// lists in the arrays given. A caller which must be reentrant supplies its
// own; the shared ones are used otherwise.
bool parse_line_into(cpp::span<char> line, struct sgrp *entry, char **admins,
                     size_t max_admins, char **members, size_t max_members);

} // namespace gshadow

namespace internal {

// The parse the flat file engine calls, which uses the shared arrays and so
// is not reentrant.
template <>
LIBC_INLINE bool parse_line<struct sgrp>(cpp::span<char> line,
                                         struct sgrp *entry) {
  return gshadow::parse_line_into(
      line, entry, gshadow::admin_list, gshadow::GSHADOW_NAME_MAX,
      gshadow::member_list, gshadow::GSHADOW_NAME_MAX);
}

} // namespace internal

namespace gshadow_db {

// Overrides the shadow group file path, for testing.
void TESTONLY_set_gshadow_path(const char *path);

// Which file the lookups read.
const char *path();

ErrorOr<void> open();
ErrorOr<void> close();
ErrorOr<struct sgrp *> read_next();

} // namespace gshadow_db
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_GSHADOW_GSHADOW_UTILS_H

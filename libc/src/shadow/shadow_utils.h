//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Declarations of helper functions and parser for shadow.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SHADOW_SHADOW_UTILS_H
#define LLVM_LIBC_SRC_SHADOW_SHADOW_UTILS_H

#include "hdr/errno_macros.h"
#include "hdr/types/struct_spwd.h"
#include "src/__support/CPP/span.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/error_or.h"
#include "src/__support/field_tokenizer.h"
#include "src/__support/flat_file_db.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/str_to_integer.h"

namespace LIBC_NAMESPACE_DECL {
namespace shadow {

constexpr size_t SHADOW_LINE_SIZE = 1024;

// Reads one of the day counts. The file leaves a field empty where there is
// no such limit, and that reads as -1.
LIBC_INLINE bool read_long_field(internal::FieldTokenizer &tokenizer,
                                 long *out) {
  auto field = tokenizer.next_field();
  if (!field)
    return false;
  if (field->empty() || field->front() == '\0') {
    *out = -1;
    return true;
  }
  auto parsed = internal::strtointeger<long>(field->data(), 10);
  if (parsed.has_error() || parsed.parsed_len <= 0 ||
      (*field)[parsed.parsed_len] != '\0')
    return false;
  *out = parsed.value;
  return true;
}

} // namespace shadow

namespace internal {

// Parses a colon separated shadow line in place. The line is
// name:hash:lstchg:min:max:warn:inact:expire:flag, and every field past the
// hash may be empty.
template <>
LIBC_INLINE bool parse_line<struct spwd>(cpp::span<char> line,
                                         struct spwd *entry) {
  if (line.empty() || !entry)
    return false;

  FieldTokenizer tokenizer(line);

  auto name = tokenizer.next_field();
  if (!name || name->empty())
    return false;
  entry->sp_namp = name->data();

  auto hash = tokenizer.next_field();
  if (!hash)
    return false;
  entry->sp_pwdp = hash->data();

  if (!shadow::read_long_field(tokenizer, &entry->sp_lstchg) ||
      !shadow::read_long_field(tokenizer, &entry->sp_min) ||
      !shadow::read_long_field(tokenizer, &entry->sp_max) ||
      !shadow::read_long_field(tokenizer, &entry->sp_warn) ||
      !shadow::read_long_field(tokenizer, &entry->sp_inact) ||
      !shadow::read_long_field(tokenizer, &entry->sp_expire))
    return false;

  long flag = 0;
  if (!shadow::read_long_field(tokenizer, &flag))
    return false;
  entry->sp_flag = static_cast<unsigned long>(flag);

  // A line with a tenth field is not a shadow entry.
  return tokenizer.exhausted();
}

} // namespace internal

namespace shadow_db {

// Overrides the shadow file path, for testing.
void TESTONLY_set_shadow_path(const char *path);

// Which file the lookups read.
const char *path();

ErrorOr<void> open();
ErrorOr<void> close();
ErrorOr<struct spwd *> read_next();

} // namespace shadow_db
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SHADOW_SHADOW_UTILS_H

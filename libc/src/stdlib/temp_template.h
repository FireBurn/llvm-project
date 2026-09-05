//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The name template the mkstemp family share: a path ending in at least six
/// 'X' characters, which are replaced with a random name.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_STDLIB_TEMP_TEMPLATE_H
#define LLVM_LIBC_SRC_STDLIB_TEMP_TEMPLATE_H

#include "hdr/errno_macros.h"
#include "hdr/stdint_proxy.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getrandom.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace temp_template {

// How many times a name is regenerated before giving up. A collision needs
// the random name to be taken already, so this is only ever reached when
// something is wrong with the directory.
constexpr int MAX_ATTEMPTS = 100;

// How many characters of the template are replaced. Any 'X' before these is
// part of the name the caller chose and is left alone.
constexpr size_t PLACEHOLDER_LENGTH = 6;

// The part of |tmpl| which is replaced: the six 'X' characters that come
// before its last |suffix_length| bytes, which are kept. An error if the
// template is not that shape.
LIBC_INLINE ErrorOr<cpp::string_view> suffix_of(char *tmpl,
                                                int suffix_length = 0) {
  if (suffix_length < 0)
    return Error(EINVAL);
  cpp::string_view view(tmpl);
  const size_t kept = static_cast<size_t>(suffix_length);
  if (view.size() < PLACEHOLDER_LENGTH + kept)
    return Error(EINVAL);
  const size_t start = view.size() - PLACEHOLDER_LENGTH - kept;
  for (size_t i = 0; i < PLACEHOLDER_LENGTH; ++i)
    if (view[start + i] != 'X')
      return Error(EINVAL);
  return cpp::string_view(tmpl + start, PLACEHOLDER_LENGTH);
}

// Replaces the run of 'X' characters with a random name. Returns the error
// number if the random bytes could not be had.
LIBC_INLINE ErrorOr<void> randomize(cpp::string_view suffix) {
  // The POSIX portable filename character set, sorted by ASCII value. See
  // https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/V1_chap03.html#tag_03_265
  static constexpr char CHARSET[] = "-._0123456789"
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                    "abcdefghijklmnopqrstuvwxyz";
  constexpr size_t CHARSET_SIZE = sizeof(CHARSET) - 1;

  char *out = const_cast<char *>(suffix.data());
  for (size_t i = 0; i < suffix.size(); ++i) {
    uint8_t byte;
    auto got = linux_syscalls::getrandom(&byte, 1, 0);
    if (!got.has_value())
      return Error(got.error());
    out[i] = CHARSET[byte % CHARSET_SIZE];
  }
  return {};
}

} // namespace temp_template
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_STDLIB_TEMP_TEMPLATE_H

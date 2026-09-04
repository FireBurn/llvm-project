//===-- Implementation of hasmntopt ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/mntent/hasmntopt.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/string/string_utils.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

bool starts_with(const char *s, const char *prefix, size_t len) {
  for (size_t i = 0; i < len; ++i)
    if (s[i] != prefix[i])
      return false;
  return true;
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(char *, hasmntopt,
                   (const struct mntent *entry, const char *opt)) {
  if (entry == nullptr || entry->mnt_opts == nullptr || opt == nullptr)
    return nullptr;

  const size_t optlen = internal::string_length(opt);
  if (optlen == 0)
    return nullptr;

  // The options are a comma separated list, and a match has to be a whole
  // option rather than part of a longer one. A value after '=' is not part
  // of the name, so it still matches.
  for (char *token = entry->mnt_opts;;) {
    if (starts_with(token, opt, optlen) &&
        (token[optlen] == '\0' || token[optlen] == '=' || token[optlen] == ','))
      return token;

    char *comma = token;
    while (*comma != '\0' && *comma != ',')
      ++comma;
    if (*comma == '\0')
      return nullptr;
    token = comma + 1;
  }
}

} // namespace LIBC_NAMESPACE_DECL

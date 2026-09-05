//===-- Implementation of getspnam ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/shadow/getspnam.h"

#include "hdr/types/struct_spwd.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/shadow/shadow_utils.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

bool same(const char *a, const char *b) {
  if (a == nullptr || b == nullptr)
    return false;
  for (; *a == *b; ++a, ++b)
    if (*a == '\0')
      return true;
  return false;
}

} // anonymous namespace

// A search rewinds the database and closes it again, so it resets any
// iteration under way with getspent. This matches what getgrnam here does.
LLVM_LIBC_FUNCTION(struct spwd *, getspnam, (const char *name)) {
  if (name == nullptr) {
    libc_errno = EINVAL;
    return nullptr;
  }
  if (auto opened = shadow_db::open(); !opened.has_value()) {
    libc_errno = opened.error();
    return nullptr;
  }
  struct spwd *found = nullptr;
  for (;;) {
    auto entry = shadow_db::read_next();
    if (!entry.has_value()) {
      libc_errno = entry.error();
      break;
    }
    if (entry.value() == nullptr)
      break; // End of file. No match is not an error.
    if (same(entry.value()->sp_namp, name)) {
      found = entry.value();
      break;
    }
  }
  shadow_db::close();
  return found;
}

} // namespace LIBC_NAMESPACE_DECL

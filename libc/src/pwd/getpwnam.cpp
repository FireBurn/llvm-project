//===-- Implementation of getpwnam ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pwd/getpwnam.h"

#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/pwd/pwd_utils.h"

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

// A search rewinds the database and closes it again, which resets any
// iteration the caller had under way with getpwent. POSIX leaves that
// unspecified so it is permitted, but glibc keeps the two independent.
// Matching that would need a second handle on the file.
LLVM_LIBC_FUNCTION(struct passwd *, getpwnam, (const char *name)) {
  if (name == nullptr) {
    libc_errno = EINVAL;
    return nullptr;
  }
  if (auto opened = passwd::open(); !opened.has_value()) {
    libc_errno = opened.error();
    return nullptr;
  }
  struct passwd *found = nullptr;
  for (;;) {
    auto entry = passwd::read_next();
    if (!entry.has_value()) {
      libc_errno = entry.error();
      break;
    }
    if (entry.value() == nullptr)
      break; // End of file, and no match is not an error.
    if (same(entry.value()->pw_name, name)) {
      found = entry.value();
      break;
    }
  }
  passwd::close();
  return found;
}

} // namespace LIBC_NAMESPACE_DECL

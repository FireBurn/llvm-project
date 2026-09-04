//===-- Implementation of getgrgid
//----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/grp/getgrgid.h"

#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/grp/grp_utils.h"

namespace LIBC_NAMESPACE_DECL {

// See the note on getgrnam: a lookup shares the one handle with getgrent.
LLVM_LIBC_FUNCTION(struct group *, getgrgid, (gid_t gid)) {
  if (auto opened = group::open(); !opened.has_value()) {
    libc_errno = opened.error();
    return nullptr;
  }
  struct group *found = nullptr;
  for (;;) {
    auto entry = group::read_next();
    if (!entry.has_value()) {
      libc_errno = entry.error();
      break;
    }
    if (entry.value() == nullptr)
      break; // End of file, and no match is not an error.
    if (entry.value()->gr_gid == gid) {
      found = entry.value();
      break;
    }
  }
  group::close();
  return found;
}

} // namespace LIBC_NAMESPACE_DECL

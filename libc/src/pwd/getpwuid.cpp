//===-- Implementation of getpwuid ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pwd/getpwuid.h"

#include "hdr/types/uid_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/pwd/pwd_utils.h"

namespace LIBC_NAMESPACE_DECL {

// See the note in getpwnam.cpp: a search resets any getpwent iteration.
LLVM_LIBC_FUNCTION(struct passwd *, getpwuid, (uid_t uid)) {
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
    if (entry.value()->pw_uid == uid) {
      found = entry.value();
      break;
    }
  }
  passwd::close();
  return found;
}

} // namespace LIBC_NAMESPACE_DECL

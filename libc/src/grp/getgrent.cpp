//===-- Implementation of getgrent
//----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/grp/getgrent.h"

#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/grp/grp_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(struct group *, getgrent, ()) {
  auto entry = group::read_next();
  if (!entry.has_value()) {
    libc_errno = entry.error();
    return nullptr;
  }
  return entry.value();
}

} // namespace LIBC_NAMESPACE_DECL

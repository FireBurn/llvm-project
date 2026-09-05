//===-- Implementation of getutxent ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/utmpx/getutxent.h"

#include "hdr/types/struct_utmpx.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/utmpx/utmpx_db.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(struct utmpx *, getutxent, (void)) {
  if (!utmpx_db::read_next())
    return nullptr;
  return &utmpx_db::entry;
}

} // namespace LIBC_NAMESPACE_DECL

//===-- Implementation of getsgent ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/gshadow/getsgent.h"

#include "hdr/types/struct_sgrp.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/gshadow/gshadow_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(struct sgrp *, getsgent, ()) {
  auto entry = gshadow_db::read_next();
  if (!entry.has_value()) {
    libc_errno = entry.error();
    return nullptr;
  }
  return entry.value();
}

} // namespace LIBC_NAMESPACE_DECL

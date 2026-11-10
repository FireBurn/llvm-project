//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of ns_name_rollback
///
//===----------------------------------------------------------------------===//

#include "src/arpa/nameser/ns_name_rollback.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/arpa/nameser/ns_name_codec.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, ns_name_rollback,
                   (const unsigned char *src, const unsigned char **dnptrs,
                    const unsigned char **lastdnptr)) {
  // Forgets the names from `src` on, which the caller has discarded.
  if (dnptrs == nullptr || lastdnptr == nullptr)
    return;
  for (const unsigned char **p = dnptrs + 1; p < lastdnptr && *p != nullptr;
       ++p) {
    if (*p >= src) {
      *p = nullptr;
      break;
    }
  }
}

} // namespace LIBC_NAMESPACE_DECL

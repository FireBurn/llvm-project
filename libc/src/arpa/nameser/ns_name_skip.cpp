//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of ns_name_skip
///
//===----------------------------------------------------------------------===//

#include "src/arpa/nameser/ns_name_skip.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/arpa/nameser/ns_name_codec.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, ns_name_skip,
                   (const unsigned char **ptr, const unsigned char *eom)) {
  if (ptr == nullptr || *ptr == nullptr || eom == nullptr) {
    libc_errno = EMSGSIZE;
    return -1;
  }
  return nameser::skip(ptr, eom);
}

} // namespace LIBC_NAMESPACE_DECL

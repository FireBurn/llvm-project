//===-- Implementation of ns_get32 ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/arpa/nameser/ns_get32.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(unsigned long, ns_get32, (const unsigned char *src)) {
  return (static_cast<unsigned long>(src[0]) << 24) |
         (static_cast<unsigned long>(src[1]) << 16) |
         (static_cast<unsigned long>(src[2]) << 8) | src[3];
}

} // namespace LIBC_NAMESPACE_DECL

//===-- Implementation of ns_get16 ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/arpa/nameser/ns_get16.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(unsigned int, ns_get16, (const unsigned char *src)) {
  return (static_cast<unsigned int>(src[0]) << 8) | src[1];
}

} // namespace LIBC_NAMESPACE_DECL

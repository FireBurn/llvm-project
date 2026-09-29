//===-- Implementation of ns_put16 ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/arpa/nameser/ns_put16.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, ns_put16, (unsigned int src, unsigned char *dst)) {
  dst[0] = static_cast<unsigned char>(src >> 8);
  dst[1] = static_cast<unsigned char>(src);
}

} // namespace LIBC_NAMESPACE_DECL

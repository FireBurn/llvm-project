//===-- Implementation of nrand48
//---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/nrand48.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/rand48.h"

namespace LIBC_NAMESPACE_DECL {

// The top thirty one bits, so the answer is never negative.
LLVM_LIBC_FUNCTION(long, nrand48, (unsigned short xsubi[3])) {
  uint64_t next = internal::rand48_step(xsubi);
  return static_cast<long>(next >> 17);
}

} // namespace LIBC_NAMESPACE_DECL

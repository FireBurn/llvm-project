//===-- Implementation of erand48
//---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/erand48.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/rand48.h"

namespace LIBC_NAMESPACE_DECL {

// A double in [0, 1), taken from the top of the forty eight bit value so
// that the best bits are the ones that survive.
LLVM_LIBC_FUNCTION(double, erand48, (unsigned short xsubi[3])) {
  uint64_t next = internal::rand48_step(xsubi);
  return static_cast<double>(next) / static_cast<double>(1ULL << 48);
}

} // namespace LIBC_NAMESPACE_DECL

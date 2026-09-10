//===-- Implementation of drand48
//---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/drand48.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/rand48.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(double, drand48, (void)) {
  uint64_t next = internal::rand48_step(internal::rand48_state.xsubi);
  return static_cast<double>(next) / static_cast<double>(1ULL << 48);
}

} // namespace LIBC_NAMESPACE_DECL

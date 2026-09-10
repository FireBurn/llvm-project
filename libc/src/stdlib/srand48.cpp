//===-- Implementation of srand48
//---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/srand48.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/rand48.h"

namespace LIBC_NAMESPACE_DECL {

// The seed goes in the high thirty two bits; the low sixteen are the value
// the standard names, so that the same seed gives the same sequence here as
// anywhere else.
LLVM_LIBC_FUNCTION(void, srand48, (long seedval)) {
  internal::rand48_unpack(
      internal::rand48_state.xsubi,
      (static_cast<uint64_t>(static_cast<uint32_t>(seedval)) << 16) |
          internal::RAND48_SEED_LOW);
  internal::rand48_state.multiplier = internal::RAND48_MULTIPLIER;
  internal::rand48_state.addend = internal::RAND48_ADDEND;
}

} // namespace LIBC_NAMESPACE_DECL

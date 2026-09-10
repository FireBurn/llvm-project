//===-- Implementation of lcong48
//---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/lcong48.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/rand48.h"

namespace LIBC_NAMESPACE_DECL {

// The only call which replaces the multiplier and the addend as well as the
// sequence: the first three words are the value, the next three the
// multiplier, and the last the addend.
LLVM_LIBC_FUNCTION(void, lcong48, (unsigned short param[7])) {
  for (int i = 0; i < 3; ++i)
    internal::rand48_state.xsubi[i] = param[i];
  internal::rand48_state.multiplier = (static_cast<uint64_t>(param[5]) << 32) |
                                      (static_cast<uint64_t>(param[4]) << 16) |
                                      static_cast<uint64_t>(param[3]);
  internal::rand48_state.addend = static_cast<uint64_t>(param[6]);
}

} // namespace LIBC_NAMESPACE_DECL

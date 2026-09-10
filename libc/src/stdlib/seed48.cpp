//===-- Implementation of seed48
//---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/seed48.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/rand48.h"

namespace LIBC_NAMESPACE_DECL {

// Sets the sequence outright and hands back what it was, in storage the
// library keeps, so a caller can put the old one back.
LLVM_LIBC_FUNCTION(unsigned short *, seed48, (unsigned short seed16v[3])) {
  static unsigned short previous[3];
  for (int i = 0; i < 3; ++i)
    previous[i] = internal::rand48_state.xsubi[i];
  for (int i = 0; i < 3; ++i)
    internal::rand48_state.xsubi[i] = seed16v[i];
  internal::rand48_state.multiplier = internal::RAND48_MULTIPLIER;
  internal::rand48_state.addend = internal::RAND48_ADDEND;
  return previous;
}

} // namespace LIBC_NAMESPACE_DECL

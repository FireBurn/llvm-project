//===-- Implementation of rand_r ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/rand_r.h"
#include "hdr/stdint_proxy.h"
#include "hdr/stdlib_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/rand_util.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, rand_r, (unsigned int *seed)) {
  // The generator is the xorshift32 rand uses where a pointer is thirty two
  // bits wide, run over the word the caller keeps rather than the library's
  // own state. Each step passes that word through the mixer a seed given to
  // srand goes through first, which spreads the bits of a caller counting up
  // from zero and keeps the state away from the all zero bits the generator
  // has no way out of.
  uint32_t x = rand_mix32(static_cast<uint32_t>(*seed));
  x ^= x >> 13;
  x ^= x << 27;
  x ^= x >> 5;
  *seed = static_cast<unsigned int>(x);
  return static_cast<int>(x * 1597334677u) & RAND_MAX;
}

} // namespace LIBC_NAMESPACE_DECL

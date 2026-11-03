//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of arc4random_uniform.
///
//===----------------------------------------------------------------------===//

#include "src/stdlib/arc4random_uniform.h"
#include "hdr/stdint_proxy.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/linux/arc4random_utils.h"

namespace LIBC_NAMESPACE_DECL {

// A remainder alone would favour the small values, so the values below the
// point where the range stops dividing evenly are thrown away.
LLVM_LIBC_FUNCTION(uint32_t, arc4random_uniform, (uint32_t upper_bound)) {
  if (upper_bound < 2)
    return 0;
  const uint32_t min = (0u - upper_bound) % upper_bound;
  uint32_t value;
  do {
    arc4random_utils::fill(&value, sizeof(value));
  } while (value < min);
  return value % upper_bound;
}

} // namespace LIBC_NAMESPACE_DECL

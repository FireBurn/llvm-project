//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of arc4random.
///
//===----------------------------------------------------------------------===//

#include "src/stdlib/arc4random.h"
#include "hdr/stdint_proxy.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/linux/arc4random_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(uint32_t, arc4random, (void)) {
  uint32_t value;
  arc4random_utils::fill(&value, sizeof(value));
  return value;
}

} // namespace LIBC_NAMESPACE_DECL

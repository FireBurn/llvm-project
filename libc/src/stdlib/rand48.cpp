//===-- The state the 48 bit random family shares -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/rand48.h"

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

// The calls which take no sequence of their own share this one.
Rand48State rand48_state;

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

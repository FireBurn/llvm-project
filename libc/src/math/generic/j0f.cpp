//===-- Implementation of j0f function ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/math/j0f.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/math/bessel.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(float, j0f, (float x)) {
  return static_cast<float>(math::j0(static_cast<double>(x)));
}

} // namespace LIBC_NAMESPACE_DECL

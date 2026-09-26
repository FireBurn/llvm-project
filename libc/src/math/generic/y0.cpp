//===-- Implementation of y0 function -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/math/y0.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/math/bessel.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(double, y0, (double x)) { return math::y0(x); }

} // namespace LIBC_NAMESPACE_DECL

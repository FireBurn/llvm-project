//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of csin function
///
//===----------------------------------------------------------------------===//

#include "src/complex/csin.h"
#include "src/__support/CPP/bit.h"
#include "src/__support/common.h"
#include "src/__support/complex_hyperbolic.h"
#include "src/__support/complex_type.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(_Complex double, csin, (_Complex double x)) {
  Complex<double> result =
      internal::complex_sin(cpp::bit_cast<Complex<double>>(x));
  return cpp::bit_cast<_Complex double>(result);
}

} // namespace LIBC_NAMESPACE_DECL

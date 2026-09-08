//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the implementation of csqrtl.
///
//===----------------------------------------------------------------------===//

#include "src/complex/csqrtl.h"
#include "src/__support/CPP/bit.h"
#include "src/__support/common.h"
#include "src/__support/complex_sqrt.h"
#include "src/__support/complex_type.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(_Complex long double, csqrtl, (_Complex long double x)) {
  Complex<long double> result =
      internal::complex_sqrt(cpp::bit_cast<Complex<long double>>(x));
  return cpp::bit_cast<_Complex long double>(result);
}

} // namespace LIBC_NAMESPACE_DECL

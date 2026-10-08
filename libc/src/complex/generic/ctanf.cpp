//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of ctanf function
///
//===----------------------------------------------------------------------===//

#include "src/complex/ctanf.h"
#include "src/__support/CPP/bit.h"
#include "src/__support/common.h"
#include "src/__support/complex_hyperbolic.h"
#include "src/__support/complex_type.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(_Complex float, ctanf, (_Complex float x)) {
  const Complex<float> in = cpp::bit_cast<Complex<float>>(x);
  const Complex<double> result = internal::complex_tan(
      {static_cast<double>(in.real), static_cast<double>(in.imag)});
  return cpp::bit_cast<_Complex float>(Complex<float>{
      static_cast<float>(result.real), static_cast<float>(result.imag)});
}

} // namespace LIBC_NAMESPACE_DECL

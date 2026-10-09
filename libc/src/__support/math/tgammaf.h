//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for tgammaf
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_MATH_TGAMMAF_H
#define LLVM_LIBC_SRC___SUPPORT_MATH_TGAMMAF_H

#include "hdr/errno_macros.h"
#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"
#include "src/__support/math/tgamma.h"

namespace LIBC_NAMESPACE_DECL {
namespace math {

// The double result is under 2^-53 relative from the true one, so rounding it
// to a float almost always rounds the true one the same way.
LIBC_INLINE float tgammaf(float x) {
  using FPBits = fputil::FPBits<float>;
  FPBits xbits(x);
  if (LIBC_UNLIKELY(xbits.is_nan() || xbits.is_inf() || xbits.is_zero() ||
                    (x < 0.0f && __builtin_floorf(x) == x))) {
    // The special values are the same in both, and carry their errno and
    // exceptions with them.
    const double d = tgamma(static_cast<double>(x));
    return static_cast<float>(d);
  }
  const double d = tgamma(static_cast<double>(x));
  const float f = static_cast<float>(d);
  if (LIBC_UNLIKELY(FPBits(f).is_inf())) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_OVERFLOW | FE_INEXACT);
  } else if (LIBC_UNLIKELY(f == 0.0f || FPBits(f).get_biased_exponent() == 0)) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_UNDERFLOW | FE_INEXACT);
  }
  return f;
}

} // namespace math
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_TGAMMAF_H

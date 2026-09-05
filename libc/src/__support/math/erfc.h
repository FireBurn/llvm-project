//===-- Implementation of the double precision erfc -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_MATH_ERFC_H
#define LLVM_LIBC_SRC___SUPPORT_MATH_ERFC_H

#include "erf_common.h"
#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"

namespace LIBC_NAMESPACE_DECL {

namespace math {

// One less the error function, which is the tail of the normal curve and is
// worked out on its own because taking it from one would leave nothing of it.
LIBC_INLINE static double erfc(double x) {
  using FPBits = fputil::FPBits<double>;
  using namespace erf_internal;

  FPBits xbits(x);
  const Sign sign = xbits.sign();
  xbits.set_sign(Sign::POS);
  const double ax = xbits.get_val();

  if (LIBC_UNLIKELY(xbits.is_inf_or_nan())) {
    if (xbits.is_nan())
      return x + x;
    // The two ends: nothing left of the tail one way, twice the whole of it
    // the other.
    return sign.is_neg() ? 2.0 : 0.0;
  }

  if (sign.is_neg()) {
    // erfc(-x) = 1 + erf(x), which is near two and loses nothing by being
    // stated as a sum.
    if (ax < 0.5)
      return 1.0 + erf_small(ax);
    if (ax >= 6.0)
      return 2.0;
    return 2.0 - erfc_positive(ax);
  }

  // Only below a half is erfc stated as one less erf: past there the two are
  // close enough that the subtraction would cost more than it is worth.
  if (ax < 0.5)
    return 1.0 - erf_small(ax);

  // Past here the tail is smaller than the smallest number there is.
  if (LIBC_UNLIKELY(ax > ERFC_UNDERFLOWS)) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_UNDERFLOW | FE_INEXACT);
    return 0.0;
  }

  const double result = erfc_positive(ax);
  // A result too small to be held with the full number of bits is an
  // underflow, and is reported as one.
  if (LIBC_UNLIKELY(FPBits(result).is_subnormal())) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_UNDERFLOW | FE_INEXACT);
  }
  return result;
}

} // namespace math

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_ERFC_H

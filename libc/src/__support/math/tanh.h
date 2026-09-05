//===-- Implementation of double precision tanh -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_MATH_TANH_H
#define LLVM_LIBC_SRC___SUPPORT_MATH_TANH_H

#include "expm1.h"
#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/FPUtil/double_double.h"
#include "src/__support/FPUtil/multiply_add.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"

namespace LIBC_NAMESPACE_DECL {

namespace tanh_internal {

// A quotient with the error of its own rounding taken back out, so what is
// left is what the exponential brought in. The sum in the denominator is
// split the same way, since it rounds too.
LIBC_INLINE double refined_div(double numerator, double denominator) {
  const fputil::DoubleDouble d = fputil::exact_add(denominator, 0.0);
  const double q = numerator / d.hi;
  const double remainder = fputil::multiply_add(-q, d.hi, numerator) - q * d.lo;
  return q + remainder / d.hi;
}

} // namespace tanh_internal

namespace math {

LIBC_INLINE static double tanh(double x) {
  using FPBits = fputil::FPBits<double>;
  FPBits xbits(x);
  const Sign sign = xbits.sign();
  xbits.set_sign(Sign::POS);
  const double ax = xbits.get_val();

  if (LIBC_UNLIKELY(xbits.is_inf_or_nan())) {
    if (xbits.is_nan())
      return x + x;
    // The two ends of the range, which tanh runs up against but never
    // reaches.
    return sign.is_neg() ? -1.0 : 1.0;
  }

  const auto with_sign = [sign](double value) {
    return sign.is_neg() ? -value : value;
  };

  // Below this tanh(x) rounds to x.
  if (LIBC_UNLIKELY(ax < 0x1.0p-28)) {
    if (ax == 0.0)
      return x;
    // The answer is not exactly x, and saying so is what the inexact flag is
    // for.
    return x - x * x * x * 0x1.5555555555555p-2;
  }

  if (ax < 22.0) {
    // tanh(x) = (e^2x - 1) / (e^2x + 1), stated through e^2x - 1 so that the
    // numerator is not the difference of two numbers near one.
    if (ax < 1.0) {
      const double t = math::expm1(-2.0 * ax);
      const fputil::DoubleDouble d = fputil::exact_add(t, 2.0);
      return with_sign(tanh_internal::refined_div(-t, d.hi) +
                       tanh_internal::refined_div(t * d.lo, d.hi * d.hi));
    }
    const double t = math::expm1(2.0 * ax);
    const fputil::DoubleDouble d = fputil::exact_add(t, 2.0);
    return with_sign(1.0 -
                     (tanh_internal::refined_div(2.0, d.hi) -
                      tanh_internal::refined_div(2.0 * d.lo, d.hi * d.hi)));
  }

  // Past that the answer is nearer one than anything else, though it never
  // reaches it, which is what the inexact flag says.
  fputil::raise_except_if_required(FE_INEXACT);
  return with_sign(1.0);
}

} // namespace math

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_TANH_H

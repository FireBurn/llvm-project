//===-- Implementation of double precision cosh -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_MATH_COSH_H
#define LLVM_LIBC_SRC___SUPPORT_MATH_COSH_H

#include "exp.h"
#include "expm1.h"
#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"

namespace LIBC_NAMESPACE_DECL {

namespace math {

LIBC_INLINE static double cosh(double x) {
  using FPBits = fputil::FPBits<double>;
  FPBits xbits(x);
  xbits.set_sign(Sign::POS);
  const double ax = xbits.get_val();

  if (LIBC_UNLIKELY(xbits.is_inf_or_nan())) {
    if (xbits.is_nan())
      return x + x;
    return FPBits::inf().get_val();
  }

  // Near zero cosh(x) is one plus half of x squared, which below this is too
  // small to tell from one. The sum is still worked out so that it is one
  // rounded up from something rather than one exactly.
  if (LIBC_UNLIKELY(ax < 0x1.0p-27))
    return 1.0 + 0.5 * ax * ax;

  // cosh(x) = 1 + t^2 / (2 (1 + t)) where t = e^|x| - 1. Stated this way the
  // answer is built up from a small quantity rather than from the difference
  // of two numbers near one, which is what costs the accuracy near zero.
  if (ax < 22.0) {
    const double t = math::expm1(ax);
    return 1.0 + t * t / (2.0 * (1.0 + t));
  }

  // Past that the second term is too small to tell, so half of e^|x| is the
  // whole answer.
  if (ax < 0x1.62e42fefa39efp+9) // log(DBL_MAX)
    return 0.5 * math::exp(ax);

  // Near the top of the range e^|x| itself overflows while half of it does
  // not, so it is squared from a half sized exponent instead.
  if (ax < 0x1.633ce8fb9f87dp+9) {
    const double half = 0.5 * math::exp(0.5 * ax);
    return half * math::exp(0.5 * ax);
  }

  fputil::set_errno_if_required(ERANGE);
  fputil::raise_except_if_required(FE_OVERFLOW);
  return FPBits::inf().get_val();
}

} // namespace math

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_COSH_H

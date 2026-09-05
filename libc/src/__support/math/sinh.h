//===-- Implementation of double precision sinh -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_MATH_SINH_H
#define LLVM_LIBC_SRC___SUPPORT_MATH_SINH_H

#include "exp.h"
#include "expm1.h"
#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"

namespace LIBC_NAMESPACE_DECL {

namespace math {

LIBC_INLINE static double sinh(double x) {
  using FPBits = fputil::FPBits<double>;
  FPBits xbits(x);
  const Sign sign = xbits.sign();
  xbits.set_sign(Sign::POS);
  const double ax = xbits.get_val();

  if (LIBC_UNLIKELY(xbits.is_inf_or_nan()))
    return x + x;

  // Below this sinh(x) rounds to x, and returning it keeps the sign and the
  // exactness of a zero.
  if (LIBC_UNLIKELY(ax < 0x1.0p-28))
    return x;

  const auto with_sign = [sign](double value) {
    return sign.is_neg() ? -value : value;
  };

  if (ax < 22.0) {
    const double t = math::expm1(ax);
    // sinh(x) = (t - t/(1+t)) / 2, which for small x is stated so that the
    // leading terms do not cancel.
    if (ax < 1.0)
      return with_sign(0.5 * (2.0 * t - t * t / (t + 1.0)));
    return with_sign(0.5 * (t + t / (t + 1.0)));
  }

  // Past that e^-|x| is too small to tell, so half of e^|x| is the answer.
  if (ax < 0x1.62e42fefa39efp+9) // log(DBL_MAX)
    return with_sign(0.5 * math::exp(ax));

  // Near the top of the range e^|x| itself overflows while half of it does
  // not, so it is squared from a half sized exponent instead.
  if (ax < 0x1.633ce8fb9f87dp+9) {
    const double half = 0.5 * math::exp(0.5 * ax);
    return with_sign(half * math::exp(0.5 * ax));
  }

  fputil::set_errno_if_required(ERANGE);
  fputil::raise_except_if_required(FE_OVERFLOW);
  return with_sign(FPBits::inf().get_val());
}

} // namespace math

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_SINH_H

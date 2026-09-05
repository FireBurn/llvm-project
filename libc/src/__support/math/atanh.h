//===-- Implementation of double precision atanh ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_MATH_ATANH_H
#define LLVM_LIBC_SRC___SUPPORT_MATH_ATANH_H

#include "log1p.h"
#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"

namespace LIBC_NAMESPACE_DECL {

namespace math {

LIBC_INLINE static double atanh(double x) {
  using FPBits = fputil::FPBits<double>;
  FPBits xbits(x);
  const Sign sign = xbits.sign();
  xbits.set_sign(Sign::POS);
  const double ax = xbits.get_val();

  if (LIBC_UNLIKELY(xbits.is_nan()))
    return x + x;

  // atanh runs out to infinity at one and is not defined past it.
  if (LIBC_UNLIKELY(ax >= 1.0)) {
    if (ax == 1.0) {
      fputil::set_errno_if_required(ERANGE);
      fputil::raise_except_if_required(FE_DIVBYZERO);
      return FPBits::inf(sign).get_val();
    }
    fputil::set_errno_if_required(EDOM);
    fputil::raise_except_if_required(FE_INVALID);
    return FPBits::quiet_nan().get_val();
  }

  // Below this atanh(x) rounds to x.
  if (LIBC_UNLIKELY(ax < 0x1.0p-28))
    return x;

  // atanh(x) = log((1 + x) / (1 - x)) / 2, stated through log1p so that the
  // quotient is not formed from two numbers near one.
  double result;
  if (ax < 0.5) {
    const double t = ax + ax;
    result = 0.5 * math::log1p(t + t * ax / (1.0 - ax));
  } else {
    result = 0.5 * math::log1p((ax + ax) / (1.0 - ax));
  }
  return sign.is_neg() ? -result : result;
}

} // namespace math

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_ATANH_H

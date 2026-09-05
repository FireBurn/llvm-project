//===-- Implementation of double precision asinh ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_MATH_ASINH_H
#define LLVM_LIBC_SRC___SUPPORT_MATH_ASINH_H

#include "log.h"
#include "log1p.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/FPUtil/sqrt.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"

namespace LIBC_NAMESPACE_DECL {

namespace math {

LIBC_INLINE static double asinh(double x) {
  using FPBits = fputil::FPBits<double>;
  FPBits xbits(x);
  const Sign sign = xbits.sign();
  xbits.set_sign(Sign::POS);
  const double ax = xbits.get_val();

  if (LIBC_UNLIKELY(xbits.is_inf_or_nan()))
    return x + x;

  // Below this asinh(x) rounds to x.
  if (LIBC_UNLIKELY(ax < 0x1.0p-28))
    return x;

  // The natural logarithm of two, to the last bit and a little beyond.
  constexpr double LOG2 = 0x1.62e42fefa39efp-1;
  double result;
  if (ax > 0x1.0p28) {
    // x^2 swamps the one, so the root is |x| and the answer is log(2|x|).
    result = math::log(ax) + LOG2;
  } else if (ax > 2.0) {
    // asinh(|x|) = log(2|x| + 1 / (sqrt(x^2 + 1) + |x|)), which keeps the
    // second term from being lost to the first.
    result =
        math::log(2.0 * ax + 1.0 / (fputil::sqrt<double>(ax * ax + 1.0) + ax));
  } else {
    // Near zero the answer is small, so it is built from log1p rather than
    // from the logarithm of something near one.
    const double t = ax * ax;
    result = math::log1p(ax + t / (1.0 + fputil::sqrt<double>(1.0 + t)));
  }
  return sign.is_neg() ? -result : result;
}

} // namespace math

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_ASINH_H

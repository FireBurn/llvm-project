//===-- Implementation of double precision acosh ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_MATH_ACOSH_H
#define LLVM_LIBC_SRC___SUPPORT_MATH_ACOSH_H

#include "log.h"
#include "log1p.h"
#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/FPUtil/sqrt.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"

namespace LIBC_NAMESPACE_DECL {

namespace math {

LIBC_INLINE static double acosh(double x) {
  using FPBits = fputil::FPBits<double>;
  FPBits xbits(x);

  if (LIBC_UNLIKELY(xbits.is_nan()))
    return x + x;

  // acosh is only defined from one upwards.
  if (LIBC_UNLIKELY(x < 1.0)) {
    fputil::set_errno_if_required(EDOM);
    fputil::raise_except_if_required(FE_INVALID);
    return FPBits::quiet_nan().get_val();
  }

  if (LIBC_UNLIKELY(xbits.is_inf()))
    return x;

  if (x == 1.0)
    return 0.0;

  // The natural logarithm of two, to the last bit and a little beyond.
  constexpr double LOG2 = 0x1.62e42fefa39efp-1;

  if (x > 0x1.0p28) {
    // x^2 swamps the one, so the root is x and the answer is log(2x).
    return math::log(x) + LOG2;
  }

  if (x > 2.0) {
    // acosh(x) = log(2x - 1 / (x + sqrt(x^2 - 1))), which keeps the second
    // term from being lost to the first.
    return math::log(2.0 * x - 1.0 / (x + fputil::sqrt<double>(x * x - 1.0)));
  }

  // Near one the answer is small, so it is built from log1p of a quantity
  // that goes to zero with it rather than from the logarithm of something
  // near one.
  const double t = x - 1.0;
  return math::log1p(t + fputil::sqrt<double>(2.0 * t + t * t));
}

} // namespace math

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_ACOSH_H

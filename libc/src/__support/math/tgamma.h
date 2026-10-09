//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for tgamma
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_MATH_TGAMMA_H
#define LLVM_LIBC_SRC___SUPPORT_MATH_TGAMMA_H

#include "hdr/errno_macros.h"
#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/FPUtil/dyadic_float.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"
#include "src/__support/math/lgamma.h"

namespace LIBC_NAMESPACE_DECL {
namespace math {

// tgamma(x) is exp(lgamma(x)) with the sign gamma has at x, which is done in
// the 128 bit dyadic floats lgamma works in and rounded once at the end.
// lgamma is under 710 in size wherever tgamma is finite, so the 128 bits leave
// the exponential some fifty bits more than a double shows.
namespace tgamma_internal {

using DFloat128 = fputil::DyadicFloat<128>;

// exp(v) for |v| < 1024: v = k log 2 + r with |r| <= log 2 / 2, the Taylor
// series for exp(r), and k added to the exponent.
LIBC_INLINE DFloat128 exp_dyadic(const DFloat128 &v) {
  const double vd = static_cast<double>(v);
  const int k = static_cast<int>(__builtin_round(vd * 0x1.71547652b82fep0));
  DFloat128 r = fputil::quick_sub(
      v, fputil::quick_mul(lgamma_internal::LN2, DFloat128(double(k))));
  DFloat128 sum(1.0);
  DFloat128 term(1.0);
  for (int n = 1; n < 60; ++n) {
    term = fputil::quick_mul(term, r);
    term = fputil::rounded_div(term, DFloat128(double(n)));
    sum = fputil::quick_add(sum, term);
    if (term.mantissa.is_zero() || term.exponent + 128 < sum.exponent - 130)
      break;
  }
  sum.exponent += k;
  return sum;
}

} // namespace tgamma_internal

LIBC_INLINE double tgamma(double x) {
  using namespace tgamma_internal;
  using FPBits = fputil::FPBits<double>;
  FPBits xbits(x);

  if (LIBC_UNLIKELY(xbits.is_nan())) {
    if (xbits.is_signaling_nan()) {
      fputil::raise_except_if_required(FE_INVALID);
      return FPBits::quiet_nan().get_val();
    }
    return x;
  }
  if (LIBC_UNLIKELY(xbits.is_inf())) {
    if (xbits.is_neg()) {
      fputil::set_errno_if_required(EDOM);
      fputil::raise_except_if_required(FE_INVALID);
      return FPBits::quiet_nan().get_val();
    }
    return x;
  }
  if (LIBC_UNLIKELY(xbits.is_zero())) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_DIVBYZERO);
    return xbits.is_neg() ? -FPBits::inf().get_val() : FPBits::inf().get_val();
  }
  if (x < 0.0 && __builtin_floor(x) == x) {
    fputil::set_errno_if_required(EDOM);
    fputil::raise_except_if_required(FE_INVALID);
    return FPBits::quiet_nan().get_val();
  }
  if (x == 1.0 || x == 2.0)
    return 1.0;

  // gamma overflows a little past 171.6, and underflows a little past -184 to
  // -186 depending on the sign, both well inside what the exponential covers,
  // so only the ends are cut.
  if (x > 172.0) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_OVERFLOW | FE_INEXACT);
    return FPBits::inf().get_val();
  }
  if (x < -190.0) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_UNDERFLOW | FE_INEXACT);
    const bool even = __builtin_fmod(__builtin_floor(x), 2.0) == 0.0;
    return even ? 0.0 : -0.0;
  }

  int sign = 1;
  DFloat128 log_gamma = lgamma_internal::lgamma_value(x, &sign);
  DFloat128 result = exp_dyadic(log_gamma);
  if (sign < 0)
    result.sign = Sign::NEG;

  const double rounded = static_cast<double>(result);
  if (LIBC_UNLIKELY(FPBits(rounded).is_inf())) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_OVERFLOW | FE_INEXACT);
  } else if (LIBC_UNLIKELY(rounded == 0.0 ||
                           FPBits(rounded).get_biased_exponent() == 0)) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_UNDERFLOW | FE_INEXACT);
  }
  return rounded;
}

} // namespace math
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_TGAMMA_H

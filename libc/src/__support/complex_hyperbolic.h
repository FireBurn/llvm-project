//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// ccosh, csinh and ctanh in double, with csin, ccos and ctan derived from
/// them. The single precision functions compute in double and round, which
/// is both accurate and cheaper than carrying a second set of cases.
///
/// The special values follow C99 Annex G.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_COMPLEX_HYPERBOLIC_H
#define LLVM_LIBC_SRC___SUPPORT_COMPLEX_HYPERBOLIC_H

#include "hdr/fenv_macros.h"
#include "src/__support/FPUtil/BasicOperations.h"
#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/FPUtil/ManipulationFunctions.h"
#include "src/__support/FPUtil/sqrt.h"
#include "src/__support/complex_type.h"
#include "src/__support/macros/config.h"
#include "src/__support/math/cos.h"
#include "src/__support/math/cosh.h"
#include "src/__support/math/exp.h"
#include "src/__support/math/sin.h"
#include "src/__support/math/sinh.h"
#include "src/__support/math/tan.h"
#include "src/__support/math/tanh.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

namespace complex_hyperbolic_detail {

using FPBits = fputil::FPBits<double>;

// cosh and sinh of a double overflow a little above 710, so past this the
// product with a cosine or sine is formed from two halves of the exponential.
constexpr double DIRECT_LIMIT = 700.0;

// Past this tanh is 1 to within what a double holds, and the imaginary part
// is small enough to be written from the exponential alone.
constexpr double TANH_LIMIT = 22.0;

LIBC_INLINE double nan() { return FPBits::quiet_nan().get_val(); }

LIBC_INLINE double inf() { return FPBits::inf().get_val(); }

LIBC_INLINE void invalid() { fputil::raise_except_if_required(FE_INVALID); }

// A zero with the sign of the product of the signs of a and b, for where the
// product itself would be infinity times zero.
LIBC_INLINE double signed_zero(double a, double b) {
  return fputil::copysign(0.0, FPBits(a).sign() == FPBits(b).sign() ? 1.0
                                                                   : -1.0);
}

LIBC_INLINE double signed_inf(double a, double b) {
  return fputil::copysign(inf(), FPBits(a).sign() == FPBits(b).sign() ? 1.0
                                                                    : -1.0);
}

} // namespace complex_hyperbolic_detail

LIBC_INLINE Complex<double> complex_cosh(Complex<double> z) {
  using namespace complex_hyperbolic_detail;
  const double x = z.real;
  const double y = z.imag;
  const FPBits xb(x);
  const FPBits yb(y);

  if (xb.is_zero()) {
    if (yb.is_inf()) {
      invalid();
      return {nan(), 0.0};
    }
    if (yb.is_nan())
      return {nan(), 0.0};
    return {math::cos(y), x * math::sin(y)};
  }

  if (xb.is_nan())
    return yb.is_zero() ? Complex<double>{nan(), 0.0}
                        : Complex<double>{nan(), nan()};

  if (xb.is_inf()) {
    if (yb.is_zero())
      return {inf(), signed_zero(x, y)};
    if (yb.is_inf()) {
      invalid();
      return {inf(), nan()};
    }
    if (yb.is_nan())
      return {inf(), nan()};
    return {fputil::copysign(inf(), math::cos(y)),
            fputil::copysign(inf(), math::sin(y) * (xb.is_neg() ? -1.0 : 1.0))};
  }

  // x is finite and not zero.
  if (!yb.is_finite()) {
    if (yb.is_inf())
      invalid();
    return {nan(), nan()};
  }

  if (yb.is_zero())
    return {math::cosh(x), signed_zero(x, y)};

  const double ax = fputil::abs(x);
  if (ax < DIRECT_LIMIT)
    return {math::cosh(x) * math::cos(y), math::sinh(x) * math::sin(y)};

  const double h = math::exp(ax * 0.5);
  return {(h * math::cos(y)) * (h * 0.5),
          ((h * math::sin(y)) * (h * 0.5)) * (xb.is_neg() ? -1.0 : 1.0)};
}

LIBC_INLINE Complex<double> complex_sinh(Complex<double> z) {
  using namespace complex_hyperbolic_detail;
  const double x = z.real;
  const double y = z.imag;
  const FPBits xb(x);
  const FPBits yb(y);

  if (xb.is_zero()) {
    if (yb.is_inf()) {
      invalid();
      return {x, nan()};
    }
    if (yb.is_nan())
      return {x, nan()};
    return {x * math::cos(y), math::sin(y)};
  }

  if (xb.is_nan())
    return yb.is_zero() ? Complex<double>{nan(), y}
                        : Complex<double>{nan(), nan()};

  if (xb.is_inf()) {
    if (yb.is_zero())
      return {x, y};
    if (yb.is_inf()) {
      invalid();
      return {x, nan()};
    }
    if (yb.is_nan())
      return {x, nan()};
    return {fputil::copysign(inf(), math::cos(y) * (xb.is_neg() ? -1.0 : 1.0)),
            fputil::copysign(inf(), math::sin(y))};
  }

  // x is finite and not zero.
  if (!yb.is_finite()) {
    if (yb.is_inf())
      invalid();
    return {nan(), nan()};
  }

  if (yb.is_zero())
    return {math::sinh(x), y};

  const double ax = fputil::abs(x);
  if (ax < DIRECT_LIMIT)
    return {math::sinh(x) * math::cos(y), math::cosh(x) * math::sin(y)};

  const double h = math::exp(ax * 0.5);
  return {((h * math::cos(y)) * (h * 0.5)) * (xb.is_neg() ? -1.0 : 1.0),
          (h * math::sin(y)) * (h * 0.5)};
}

LIBC_INLINE Complex<double> complex_tanh(Complex<double> z) {
  using namespace complex_hyperbolic_detail;
  const double x = z.real;
  const double y = z.imag;
  const FPBits xb(x);
  const FPBits yb(y);

  if (xb.is_nan())
    return yb.is_zero() ? Complex<double>{nan(), y}
                        : Complex<double>{nan(), nan()};

  if (xb.is_inf()) {
    // The real part is +-1 whatever the imaginary part is, and the imaginary
    // part is a zero with the sign of sin(2y) where that has one.
    const double sign = xb.is_neg() ? -1.0 : 1.0;
    if (yb.is_finite())
      return {sign, fputil::copysign(0.0, math::sin(2.0 * y))};
    return {sign, fputil::copysign(0.0, y)};
  }

  // x is finite from here.
  if (!yb.is_finite()) {
    if (yb.is_inf())
      invalid();
    // A zero real part keeps its sign, as it does in glibc and musl.
    return xb.is_zero() ? Complex<double>{x, nan()}
                        : Complex<double>{nan(), nan()};
  }

  if (yb.is_zero())
    return {math::tanh(x), y};

  const double ax = fputil::abs(x);
  if (ax >= TANH_LIMIT) {
    const double decay = math::exp(-ax);
    return {fputil::copysign(1.0, x),
            4.0 * math::sin(y) * math::cos(y) * decay * decay};
  }

  // With t = tan y and s = sinh x, tanh(x + iy) = (beta rho s + i t) / (1 +
  // beta s^2) where beta = 1 + t^2 and rho = sqrt(1 + s^2). Nothing in it
  // cancels, which the quotient of sinh(2x) and cos(2y) would.
  const double t = math::tan(y);
  const double beta = 1.0 + t * t;
  const double s = math::sinh(x);
  const double rho = fputil::sqrt<double>(1.0 + s * s);
  const double denom = 1.0 + beta * s * s;
  return {beta * rho * s / denom, t / denom};
}

// sin(z) = -i sinh(iz), cos(z) = cosh(iz) and tan(z) = -i tanh(iz). For
// z = x + iy, iz = -y + ix, and -i(a + ib) = b - ia.
LIBC_INLINE Complex<double> complex_sin(Complex<double> z) {
  const Complex<double> r = complex_sinh({-z.imag, z.real});
  return {r.imag, -r.real};
}

LIBC_INLINE Complex<double> complex_cos(Complex<double> z) {
  return complex_cosh({-z.imag, z.real});
}

LIBC_INLINE Complex<double> complex_tan(Complex<double> z) {
  const Complex<double> r = complex_tanh({-z.imag, z.real});
  return {r.imag, -r.real};
}

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_COMPLEX_HYPERBOLIC_H

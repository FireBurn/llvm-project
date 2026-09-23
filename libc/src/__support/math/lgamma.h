//===-- Natural logarithm of the gamma function -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_MATH_LGAMMA_H
#define LLVM_LIBC_SRC___SUPPORT_MATH_LGAMMA_H

#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/FPUtil/PolyEval.h"
#include "src/__support/FPUtil/multiply_add.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"
#include "src/__support/math/log.h"
#include "src/__support/math/sin.h"

namespace LIBC_NAMESPACE_DECL {
namespace math {

namespace lgamma_internal {

// The Lanczos approximation, which states that for g chosen in advance and
// Re(z) > 0,
//
//   gamma(z+1) = sqrt(2*pi) * (z + g + 1/2)^(z+1/2) * e^-(z+g+1/2) * A(z)
//
// with A(z) a short sum whose coefficients follow from g. Taking logarithms
// of both sides gives lgamma without ever forming gamma itself, which would
// overflow almost at once.
//
// g = 6.024680040776729583740234375 with fourteen terms, the parameters
// Boost derives for double work, which hold the relative error of A(z) to
// around 2^-52 over the half plane.
LIBC_INLINE_VAR constexpr double LANCZOS_G = 6.024680040776729583740234375;

LIBC_INLINE_VAR constexpr double LANCZOS_NUM[13] = {
    23531376880.410759688572007674,  42919803642.649098768957899047,
    35711959237.355668049440185451,  17921034426.037209699919755754,
    6039542586.3520280050642916443,  1439720407.3117216736632230727,
    248874557.86205415651146038641,  31426415.585400194380614231628,
    2876370.6289353724412254090516,  186056.26539522349504029498971,
    8071.6720023658162106380029022,  210.82427775157934587250973392,
    2.5066282746310002701649081771,
};

LIBC_INLINE_VAR constexpr double LANCZOS_DEN[13] = {
    0.0,          39916800.0,   120543840.0, 150917976.0, 105258076.0,
    45995730.0,   13339535.0,   2637558.0,   357423.0,    32670.0,
    1925.0,       66.0,         1.0,
};

// log(sqrt(2*pi)), for Stirling's series.
LIBC_INLINE_VAR constexpr double LOG_SQRT_2PI = 0x1.d67f1c864beb5p-1;
LIBC_INLINE_VAR constexpr double PI = 0x1.921fb54442d18p+1;

// The coefficients already carry sqrt(2*pi), so this is the whole of the
// factor the approximation multiplies by.
LIBC_INLINE double lanczos_sum(double z) {
  double num = 0.0, den = 0.0;
  for (int i = 12; i >= 0; --i) {
    num = fputil::multiply_add(num, z, LANCZOS_NUM[i]);
    den = fputil::multiply_add(den, z, LANCZOS_DEN[i]);
  }
  return num / den;
}

// lgamma is zero at one and two, so there an error the Lanczos form
// makes in absolute terms is large in relative ones. Near them the
// Taylor series are used instead, from zeta(k), which converge fast
// enough for thirty terms to cover a quarter either side of one, and
// sixty three quarters either side of two.
//   lgamma(1+e) = -gamma e + sum (-1)^k zeta(k)/k e^k
//   lgamma(2+e) = (1-gamma) e + sum (-1)^k (zeta(k)-1)/k e^k
LIBC_INLINE_VAR constexpr double SERIES_AT_1[30] = {
    -0x1.2788cfc6fb619p-1,
    0x1.a51a6625307d3p-1,
    -0x1.9a4d55beab2d7p-2,
    0x1.151322ac7d848p-2,
    -0x1.a8b9c17aa6149p-3,
    0x1.5b40cb100c306p-3,
    -0x1.2703a1dcea3aep-3,
    0x1.010b36af86397p-3,
    -0x1.c806706d57db4p-4,
    0x1.9a01e385d5f8fp-4,
    -0x1.748c33114c6d6p-4,
    0x1.556ad63243bc4p-4,
    -0x1.3b1d971fc5985p-4,
    0x1.2496df8320c5fp-4,
    -0x1.11133476e7fep-4,
    0x1.00010064cdeb2p-4,
    -0x1.e1e2d311e8abdp-5,
    0x1.c71ce3a20b419p-5,
    -0x1.af28a1b5688ap-5,
    0x1.9999b3352d5bap-5,
    -0x1.86186db77bfbfp-5,
    0x1.745d1d1778df9p-5,
    -0x1.642c88591b66dp-5,
    0x1.555556aaafdcdp-5,
    -0x1.47ae151eb9fb7p-5,
    0x1.3b13b189d925ep-5,
    -0x1.2f684c00002bcp-5,
    0x1.24924936db7bcp-5,
    -0x1.1a7b961a7b9aap-5,
    0x1.111111155556dp-5,
};

LIBC_INLINE_VAR constexpr double SERIES_AT_2[60] = {
    0x1.b0ee6072093cep-2,
    0x1.4a34cc4a60fa6p-2,
    -0x1.13e001a557607p-4,
    0x1.51322ac7d8483p-6,
    -0x1.e404fc218f5f2p-8,
    0x1.7add6eadb6c3p-9,
    -0x1.38ac5c2bf8e08p-10,
    0x1.0b36af86396e9p-11,
    -0x1.d3fd4c76d2fc8p-13,
    0x1.a127b0f17d65ap-14,
    -0x1.78de5bd7c81efp-15,
    0x1.580dcee66eb02p-16,
    -0x1.3cbc963ce2243p-17,
    0x1.2597a39f34aacp-18,
    -0x1.11b2eb7679541p-19,
    0x1.0064cdeb22f0fp-20,
    -0x1.e2600d93cfd2fp-22,
    0x1.c76bbb3f07a4dp-23,
    -0x1.af5a6cbbf8a97p-24,
    0x1.99b93c2070b0fp-25,
    -0x1.862c734df3eacp-26,
    0x1.7469daccfadcdp-27,
    -0x1.6434a8447aeadp-28,
    0x1.555a877ffd2c3p-29,
    -0x1.47b1679258d0ep-30,
    0x1.3b15d2b2fc10cp-31,
    -0x1.2f69a9fabe3ep-32,
    0x1.24932a337434cp-33,
    -0x1.1a7c26ec2523cp-34,
    0x1.11116e693ed98p-35,
    -0x1.08424cbc543d8p-36,
    0x1.000026e3f644fp-37,
    -0x1.f07c514fc9f0ap-39,
    0x1.e1e2026aafcd8p-40,
    -0x1.d41d56e5ee2e2p-41,
    0x1.c71c7f6f10e37p-42,
    -0x1.bacf9a27bc89bp-43,
    0x1.af28718a10d6ep-44,
    -0x1.a41a45603e5b6p-45,
    0x1.99999c0716ee9p-46,
    -0x1.8f9c1a8df9d78p-47,
    0x1.8618628d28905p-48,
    -0x1.7d05f4c31c56p-49,
    0x1.745d17b56ba4ap-50,
    -0x1.6c16c1b4d6456p-51,
    0x1.642c85c023d9dp-52,
    -0x1.5c9882d825e9dp-53,
    0x1.555555698a866p-54,
    -0x1.4e5e0a8022bc9p-55,
    0x1.47ae14838081fp-56,
    -0x1.41414146e3e31p-57,
    0x1.3b13b13ec2f3ap-58,
    -0x1.3521cfb520859p-59,
    0x1.2f684bdba6a99p-60,
    -0x1.29e4129f49674p-61,
    0x1.249249253f4ccp-62,
    -0x1.1f7047dc8348bp-63,
    0x1.1a7b9611f1f7cp-64,
    -0x1.15b1e5f78318fp-65,
    0x1.1111111130f6cp-66,
};

template <int N> LIBC_INLINE double series(const double (&c)[N], double e) {
  double p = c[N - 1];
  for (int i = N - 2; i >= 0; --i)
    p = fputil::multiply_add(p, e, c[i]);
  return p * e;
}

LIBC_INLINE double lgamma_positive(double x) {
  // Below three quarters, lgamma(x) = lgamma(x+1) - log(x) moves it into
  // the series below without cancelling, since -log(x) dominates there.
  if (x < 0.75)
    return lgamma_positive(x + 1.0) - math::log(x);
  if (x <= 1.25)
    return series(SERIES_AT_1, x - 1.0);
  if (x <= 2.75)
    return series(SERIES_AT_2, x - 2.0);
  // One step of lgamma(x) = log(x-1) + lgamma(x-1). Both subtractions are
  // exact here.
  if (x <= 3.75)
    return math::log(x - 1.0) + series(SERIES_AT_2, x - 3.0);

  // The sum above overflows long before its ratio does, and by here the
  // first correction term of Stirling's series is already below what a
  // double can show of the result.
  if (x >= 0x1.0p27) {
    double lx = math::log(x);
    return fputil::multiply_add(x - 0.5, lx, LOG_SQRT_2PI - x) +
           1.0 / (12.0 * x);
  }

  double t = x + LANCZOS_G - 0.5;
  return fputil::multiply_add(x - 0.5, math::log(t), -t) +
         math::log(lanczos_sum(x));
}

} // namespace lgamma_internal

// The sign of gamma at x, which lgamma reports separately since it returns
// the logarithm of the magnitude. Gamma is negative on exactly every other
// interval below zero.
LIBC_INLINE double lgamma_r(double x, int *signp) {
  using namespace lgamma_internal;
  using FPBits = fputil::FPBits<double>;

  FPBits xbits(x);
  *signp = 1;

  if (LIBC_UNLIKELY(xbits.is_nan())) {
    if (xbits.is_signaling_nan()) {
      fputil::raise_except_if_required(FE_INVALID);
      return FPBits::quiet_nan().get_val();
    }
    return x;
  }
  if (LIBC_UNLIKELY(xbits.is_inf()))
    return FPBits::inf().get_val();

  if (LIBC_UNLIKELY(xbits.is_zero())) {
    *signp = xbits.is_neg() ? -1 : 1;
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_DIVBYZERO);
    return FPBits::inf().get_val();
  }

  if (x > 0.0)
    return lgamma_positive(x);

  // A pole at every whole number at or below zero.
  double floor_x = __builtin_floor(x);
  if (floor_x == x) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_DIVBYZERO);
    return FPBits::inf().get_val();
  }

  // Reflection: gamma(x) * gamma(1-x) = pi / sin(pi*x), so
  //   lgamma(x) = log(pi / |sin(pi*x)|) - lgamma(1-x).
  // The sine is taken of the fractional part alone, since pi*x loses all
  // meaning once x is large, and the sign of gamma follows from which
  // interval x fell in.
  double fractional = x - floor_x;         // in (0, 1)
  double sin_term = math::sin(PI * fractional);
  // gamma alternates sign below zero: negative on (-1,0), positive on
  // (-2,-1), and so on.
  bool floor_is_even = (__builtin_floor(floor_x * 0.5) * 2.0 == floor_x);
  *signp = floor_is_even ? 1 : -1;

  return math::log(PI / __builtin_fabs(sin_term)) - lgamma_positive(1.0 - x);
}

LIBC_INLINE double lgamma(double x) {
  int sign;
  return lgamma_r(x, &sign);
}

} // namespace math
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_LGAMMA_H

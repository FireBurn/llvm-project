//===-- log2 of a long double to full precision -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_MATH_LOG2L_IMPL_H
#define LLVM_LIBC_SRC___SUPPORT_MATH_LOG2L_IMPL_H

#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/FPUtil/double_double.h"
#include "src/__support/FPUtil/multiply_add.h"
#include "src/__support/FPUtil/dyadic_float.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"
#include "src/__support/macros/properties/types.h"
#include "src/__support/math/log2.h"

namespace LIBC_NAMESPACE_DECL {
namespace math {

// log2 of a long double, to more precision than a long double holds.
//
// An x86 long double carries sixty four bits of mantissa, which is eleven
// more than a double. Splitting it gives two doubles whose sum is the
// original exactly, since between them they have every bit:
//
//   x = hi + lo,  |lo| <= 2^-53 |hi|
//
// and then
//
//   log2(x) = log2(hi) + log2(1 + lo/hi).
//
// The first term is what the double routine's accurate pass computes, taken
// before it is rounded. For the second, t = lo/hi is carried as a sum of two
// doubles and the series taken to t^3, which is enough: with |t| <= 2^-53 the
// first term left out is below 2^-210 while the result is at least 2^-65.
LIBC_INLINE fputil::DyadicFloat<128> log2_dyadic_long(long double x) {
  using namespace log2_internal;
  using namespace common_constants_internal;
  using FPBits_t = fputil::FPBits<double>;

  // The exponent range of a long double is far wider than a double's, so
  // take the exponent out first and split only the mantissa.
  using LDBits = fputil::FPBits<long double>;
  int x_e = 0;
  if (LIBC_UNLIKELY(LDBits(x).is_subnormal())) {
    x *= 0x1.0p64L;
    x_e -= 64;
  }
  LDBits xbits(x);
  x_e += xbits.get_exponent();
  xbits.set_biased_exponent(LDBits::EXP_BIAS);
  long double xm = xbits.get_val();

  double hi = static_cast<double>(xm);
  double lo = static_cast<double>(xm - static_cast<long double>(hi));

  // The decomposition the double routine performs on its own argument.
  uint64_t x_u = FPBits_t(hi).uintval();
  x_e -= FPBits_t::EXP_BIAS;

  int index = static_cast<int>(x_u >> 45) & 0x7F;
  x_e += static_cast<int>((x_u + (1ULL << 45)) >> 52);

  uint64_t x_m = (x_u & 0x000F'FFFF'FFFF'FFFFULL) | 0x3FF0'0000'0000'0000ULL;
  double m = FPBits_t(x_m).get_val();

  // The reduced argument the accurate pass expects, which is the mantissa
  // brought close to one by the reciprocal the table is built around, not
  // the mantissa itself.
  double rd = RD[index];
#ifdef LIBC_TARGET_CPU_HAS_FMA_DOUBLE
  double u = fputil::multiply_add(rd, m, -1.0); // exact
#else
  uint64_t c_m = x_m & 0x3FFF'E000'0000'0000ULL;
  double c = FPBits_t(c_m).get_val();
  double u = fputil::multiply_add(rd, m - c, CD[index]); // exact
#endif // LIBC_TARGET_CPU_HAS_FMA_DOUBLE

  DFloat128 r = log2_dyadic(x_e, index, u);

  if (lo != 0.0) {
    constexpr DFloat128 ONE_OVER_LN2 = {
        Sign::POS, -127, 0xb8aa3b29'5c17f0bb'be87fed0'691d3e89_u128};
    constexpr DFloat128 ONE_THIRD = {
        Sign::POS, -129, 0xaaaaaaaa'aaaaaaaa'aaaaaaaa'aaaaaaab_u128};
    double t_hi = lo / hi;
    fputil::DoubleDouble q = fputil::exact_mult(t_hi, hi);
    double t_lo = ((lo - q.hi) - q.lo) / hi;
    DFloat128 t = fputil::quick_add(DFloat128(t_hi), DFloat128(t_lo));
    DFloat128 t2 = fputil::quick_mul(t, t);
    // t - t^2/2 + t^3/3
    DFloat128 poly = fputil::quick_mul(t2, ONE_THIRD);
    poly = fputil::quick_mul(t, poly);
    DFloat128 half_t2 = t2;
    half_t2.exponent -= 1;
    half_t2.sign = Sign::NEG;
    poly = fputil::quick_add(poly, half_t2);
    poly = fputil::quick_add(t, poly);
    r = fputil::quick_add(r, fputil::quick_mul(poly, ONE_OVER_LN2));
  }

  return r;
}

} // namespace math
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_LOG2L_IMPL_H

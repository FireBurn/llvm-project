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
#include "src/__support/FPUtil/multiply_add.h"
#include "src/__support/FPUtil/dyadic_float.h"
#include "src/__support/macros/config.h"
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
// before it is rounded. The second is small enough that one term of its
// series is more than the answer can show: with |t| <= 2^-53 the next term
// is of the order of 2^-107, while the result needs only sixty four bits.
LIBC_INLINE fputil::DyadicFloat<128> log2_dyadic_long(long double x) {
  using namespace log2_internal;
  using namespace common_constants_internal;
  using FPBits_t = fputil::FPBits<double>;

  double hi = static_cast<double>(x);
  double lo = static_cast<double>(x - static_cast<long double>(hi));

  // The decomposition the double routine performs on its own argument.
  FPBits_t hibits(hi);
  uint64_t x_u = hibits.uintval();
  int x_e = -FPBits_t::EXP_BIAS;

  if (LIBC_UNLIKELY(x_u < FPBits_t::min_normal().uintval())) {
    // Subnormal, which the tables do not cover. Scale it up and account for
    // the scaling in the exponent.
    hibits = FPBits_t(hi * 0x1.0p52);
    x_e -= 52;
    x_u = hibits.uintval();
  }

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
    // log2(1 + t) with t = lo/hi. One over the natural logarithm of two,
    // to more places than a double holds, so the correction is not the
    // thing that limits the result.
    constexpr DFloat128 ONE_OVER_LN2 = {Sign::POS, -127,
                                  0xb8aa3b295c17f0bb'be87fed0691d3e88_u128};
    double t = lo / hi;
    r = fputil::quick_add(r, fputil::quick_mul(DFloat128(t), ONE_OVER_LN2));
  }

  return r;
}

} // namespace math
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_LOG2L_IMPL_H

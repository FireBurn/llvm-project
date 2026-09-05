//===-- What erf and erfc are both built from -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_MATH_ERF_COMMON_H
#define LLVM_LIBC_SRC___SUPPORT_MATH_ERF_COMMON_H

#include "exp.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/FPUtil/PolyEval.h"
#include "src/__support/FPUtil/multiply_add.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"

namespace LIBC_NAMESPACE_DECL {

namespace erf_internal {

// erf(x) = x P(x^2) for |x| <= 1. Chebyshev interpolation of erf(x)/x over
// x^2 in [0, 1], which holds the relative error under 2^-55.
LIBC_INLINE_VAR constexpr double ERF_POLY[12] = {
    0x1.20dd750429b6dp+0,   -0x1.812746b0379dbp-2,  0x1.ce2f21a041d19p-4,
    -0x1.b82ce3122f71bp-6,  0x1.565bccfec166cp-8,   -0x1.c02db0ed7d7c0p-11,
    0x1.f9a2c804a6c17p-14,  -0x1.f4caf2c048eaep-17, 0x1.b984e2c3e7367p-20,
    -0x1.5c1f3b1fd2950p-23, 0x1.d798878b40fdep-27,  -0x1.acbabf57ef501p-31};

// Past one the answer is stated as erfc(x) = e^(-x^2) G(x) / x, where G is
// what the three tables below approximate over their own stretch of the
// range. G is slowly varying, which is what makes it worth pulling the
// exponential out.

// 1/2 <= x < 2, in x - 5/4. It starts below one so that erfc need not be
// stated as one less erf anywhere the two are close enough for the
// subtraction to cost bits.
LIBC_INLINE_VAR constexpr double ERFC_POLY_1[20] = {
    0x1.d6d0369869419p-2,   0x1.b56f45eef7e58p-4,   -0x1.348a84149b8cap-4,
    0x1.68a25a6641f7dp-5,   -0x1.73fbc48c6dfe1p-6,  0x1.5d48f04008618p-7,
    -0x1.301a5025c1369p-8,  0x1.f0fe6fb8febbbp-10,  -0x1.80788bcd31134p-11,
    0x1.1b6620ffcc4adp-12,  -0x1.901b08eee6160p-14, 0x1.0f92a60833667p-15,
    -0x1.63a3603060572p-17, 0x1.c295e90070094p-19,  -0x1.1537aad3a1962p-20,
    0x1.4adecd7242e5cp-22,  -0x1.76b36511cd7ddp-24, 0x1.a9723979e6ffdp-26,
    -0x1.29791e345de1ep-27, 0x1.3ec8465611ba8p-29};

// 2 <= x < 4, in x - 3.
LIBC_INLINE_VAR constexpr double ERFC_POLY_2[19] = {
    0x1.12f21ddd9f5e2p-1,   0x1.043fe1a98c0e7p-6,   -0x1.b8589297c81f1p-8,
    0x1.409cc2ed3d229p-9,   -0x1.a8d67c44a961dp-11, 0x1.06d6ae44cbe60p-12,
    -0x1.34206e35bbf1bp-14, 0x1.595f1ab8339f0p-16,  -0x1.7476ee501105ep-18,
    0x1.843f6f5856cc4p-20,  -0x1.88869dc26f939p-22, 0x1.81f2b7c1eb801p-24,
    -0x1.71eda858bdebfp-26, 0x1.5ad304caf0e5cp-28,  -0x1.3d7787379225fp-30,
    0x1.14dd7f577feb9p-32,  -0x1.e7c70d228a407p-35, 0x1.0d8a8d59064a4p-36,
    -0x1.c5c32eea482bbp-39};

// 4 <= x, in 1 / x^2, which is where G flattens out towards 1 / sqrt(pi).
LIBC_INLINE_VAR constexpr double ERFC_POLY_3[14] = {
    0x1.20dd750429b6cp-1,   -0x1.20dd75042908cp-2, 0x1.b14c2f84660dbp-2,
    -0x1.0ecf9d1484e00p+0,  0x1.d9eb164145d11p+1,  -0x1.0a909ca64c3a9p+4,
    0x1.6e3d7fb26cffdp+6,   -0x1.2794c0da7935cp+9, 0x1.0b6c67f684cd1p+12,
    -0x1.f59cbd371b2c3p+14, 0x1.b24dce43791e3p+17, -0x1.2d6d61a98a95ap+20,
    0x1.19515a1ea75bap+22,  -0x1.feb1d3d76f2ecp+22};

// Past this erfc is smaller than the smallest number there is.
LIBC_INLINE_VAR constexpr double ERFC_UNDERFLOWS = 0x1.b39dc41e48bfdp+4;

// erf(x) for |x| <= 1, where the answer is small and has to be built up from
// x rather than come out of a difference.
LIBC_INLINE double erf_small(double x) {
  const double t = x * x;
  const double p =
      fputil::polyeval(t, ERF_POLY[0], ERF_POLY[1], ERF_POLY[2], ERF_POLY[3],
                       ERF_POLY[4], ERF_POLY[5], ERF_POLY[6], ERF_POLY[7],
                       ERF_POLY[8], ERF_POLY[9], ERF_POLY[10], ERF_POLY[11]);
  return x * p;
}

// erfc(x) for x >= 1/2, where the answer runs away towards zero and it is
// the relative accuracy that has to hold.
LIBC_INLINE double erfc_positive(double x) {
  using FPBits = fputil::FPBits<double>;

  // e^(-x^2), computed so that the rounding of x^2 does not scale up into the
  // answer. With the low bits of x cleared, x_hi^2 is exact, and what is left
  // over is small enough to be taken by a few terms of the series rather than
  // by a second exponential, which saves its rounding.
  const FPBits hi_bits(FPBits(x).uintval() & ~FPBits::StorageType(0x7FF'FFFF));
  const double x_hi = hi_bits.get_val();
  const double x_lo = x - x_hi;
  // At most 2^-26 x^2, so under 2^-16 over the range this is reached with.
  const double e = x_lo * (x + x_hi);
  const double correction =
      e *
      fputil::multiply_add(e, fputil::multiply_add(e, -1.0 / 6.0, 0.5), -1.0);
  const double exp_hi = math::exp(-x_hi * x_hi);
  const double factor = fputil::multiply_add(exp_hi, correction, exp_hi);

  double g;
  if (x < 2.0) {
    const double s = x - 1.25;
    g = fputil::polyeval(
        s, ERFC_POLY_1[0], ERFC_POLY_1[1], ERFC_POLY_1[2], ERFC_POLY_1[3],
        ERFC_POLY_1[4], ERFC_POLY_1[5], ERFC_POLY_1[6], ERFC_POLY_1[7],
        ERFC_POLY_1[8], ERFC_POLY_1[9], ERFC_POLY_1[10], ERFC_POLY_1[11],
        ERFC_POLY_1[12], ERFC_POLY_1[13], ERFC_POLY_1[14], ERFC_POLY_1[15],
        ERFC_POLY_1[16], ERFC_POLY_1[17], ERFC_POLY_1[18], ERFC_POLY_1[19]);
  } else if (x < 4.0) {
    const double s = x - 3.0;
    g = fputil::polyeval(
        s, ERFC_POLY_2[0], ERFC_POLY_2[1], ERFC_POLY_2[2], ERFC_POLY_2[3],
        ERFC_POLY_2[4], ERFC_POLY_2[5], ERFC_POLY_2[6], ERFC_POLY_2[7],
        ERFC_POLY_2[8], ERFC_POLY_2[9], ERFC_POLY_2[10], ERFC_POLY_2[11],
        ERFC_POLY_2[12], ERFC_POLY_2[13], ERFC_POLY_2[14], ERFC_POLY_2[15],
        ERFC_POLY_2[16], ERFC_POLY_2[17], ERFC_POLY_2[18]);
  } else {
    const double u = 1.0 / (x * x);
    g = fputil::polyeval(u, ERFC_POLY_3[0], ERFC_POLY_3[1], ERFC_POLY_3[2],
                         ERFC_POLY_3[3], ERFC_POLY_3[4], ERFC_POLY_3[5],
                         ERFC_POLY_3[6], ERFC_POLY_3[7], ERFC_POLY_3[8],
                         ERFC_POLY_3[9], ERFC_POLY_3[10], ERFC_POLY_3[11],
                         ERFC_POLY_3[12], ERFC_POLY_3[13]);
  }
  return factor * g / x;
}

} // namespace erf_internal

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_ERF_COMMON_H

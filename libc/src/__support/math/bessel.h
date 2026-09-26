//===-- Bessel functions of the first and second kind -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_MATH_BESSEL_H
#define LLVM_LIBC_SRC___SUPPORT_MATH_BESSEL_H

#include "range_reduction_double_common.h"
#include "sincos_eval.h"
#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/FPUtil/dyadic_float.h"
#include "src/__support/FPUtil/sqrt.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"
#include "src/__support/math/log2l_impl.h"

#ifdef LIBC_TARGET_CPU_HAS_FMA_DOUBLE
#include "range_reduction_double_fma.h"
#else
#include "range_reduction_double_nofma.h"
#endif // LIBC_TARGET_CPU_HAS_FMA_DOUBLE

namespace LIBC_NAMESPACE_DECL {
namespace math {

namespace bessel_internal {

using DFloat128 = fputil::DyadicFloat<128>;

LIBC_INLINE_VAR constexpr DFloat128 TWO_OVER_PI = {
    Sign::POS, -128, 0xa2f9836e'4e441529'fc2757d1'f534ddc1_u128};
LIBC_INLINE_VAR constexpr DFloat128 ONE_OVER_PI = {
    Sign::POS, -129, 0xa2f9836e'4e441529'fc2757d1'f534ddc1_u128};
LIBC_INLINE_VAR constexpr DFloat128 EULER = {
    Sign::POS, -128, 0x93c467e3'7db0c7a4'd1be3f81'0152cb57_u128};
LIBC_INLINE_VAR constexpr DFloat128 LN2 = {
    Sign::POS, -128, 0xb17217f7'd1cf79ab'c9e3b398'03f2f6af_u128};

// Below this the power series are summed, above it the asymptotic
// expansions. At 32 the series lose 46 of their 128 bits to cancellation,
// and the smallest term of the asymptotic series is below 2^-90.
LIBC_INLINE_VAR constexpr double SERIES_LIMIT = 32.0;

using DFloat256 = fputil::DyadicFloat<256>;

// The binary exponent of the leading bit, so that 2^magnitude(v) <= |v|.
template <size_t Bits>
LIBC_INLINE int magnitude(const fputil::DyadicFloat<Bits> &v) {
  return v.exponent + static_cast<int>(Bits) - 1;
}

LIBC_INLINE DFloat128 narrow(DFloat256 v) {
  v.normalize();
  return DFloat128::round(v.sign, v.exponent + 128, v.mantissa, 128);
}

// rounded_div expects normalized operands, which sums after cancellation
// need not be, and a positive divisor, since its reciprocal iteration
// diverges for a negative one.
template <size_t Bits>
LIBC_INLINE fputil::DyadicFloat<Bits> div(fputil::DyadicFloat<Bits> a,
                                          fputil::DyadicFloat<Bits> b) {
  a.normalize();
  b.normalize();
  bool neg = b.sign.is_neg();
  b.sign = Sign::POS;
  fputil::DyadicFloat<Bits> q = fputil::rounded_div(a, b);
  if (neg)
    q.sign = q.sign.negate();
  return q;
}

template <typename DF = DFloat128>
LIBC_INLINE DF from_int(unsigned long long n) {
  return DF(static_cast<long double>(n));
}

// log(v) for v > 0, to about 2^-120: log2 of a long double hi near v, and
// log(1 + lo/hi) as lo/hi for what is left.
LIBC_INLINE DFloat128 log_dyadic(const DFloat128 &v) {
  long double hi = static_cast<long double>(v);
  DFloat128 r = fputil::quick_mul(log2_dyadic_long(hi), LN2);
  DFloat128 lo = fputil::quick_sub(v, DFloat128(hi));
  if (!lo.mantissa.is_zero())
    r = fputil::quick_add(r, fputil::quick_mul(lo, DFloat128(1.0L / hi)));
  return r;
}

// sqrt(v) for v > 0: the long double root and one Newton step.
LIBC_INLINE DFloat128 sqrt_dyadic(const DFloat128 &v) {
  DFloat128 s(fputil::sqrt<long double>(static_cast<long double>(v)));
  DFloat128 q = div(v, s);
  return fputil::mul_pow_2(fputil::quick_add(s, q), -1);
}

// The sums of the series about zero. With h = x/2 and t = h^2,
//
//   J0(x) = sum (-t)^k / k!^2
//   J1(x) = sum h (-t)^k / (k! (k+1)!)
//   Y0(x) = 2/pi ((log h + gamma) J0(x) + S0)
//   Y1(x) = -2/(pi x) + 2/pi (log h + gamma) J1(x) - S1/pi
//
// where H_k is the kth harmonic number and
//
//   S0 = - sum H_k (-t)^k / k!^2
//   S1 = sum (H_k + H_{k+1}) h (-t)^k / (k! (k+1)!)
struct Series {
  DFloat128 j0, j1, s0, s1;
};

template <typename DF> struct SeriesOf {
  DF j0, j1, s0, s1;
};

// The terms grow to about e^x / sqrt(2 pi x) before they fall, so the sums
// lose that many bits to cancellation. floor is where they stop mattering.
template <typename DF>
LIBC_INLINE SeriesOf<DF> series_sum(double x, bool with_y, int floor) {
  DF h = fputil::mul_pow_2(DF(x), -1);
  DF mt = fputil::quick_mul(h, h);
  mt.sign = Sign::NEG;
  DF term0(1.0), term1 = h;
  DF harmonic{}, harmonic_next(1.0);
  SeriesOf<DF> r{term0, term1, DF(), term1};
  for (unsigned k = 1;; ++k) {
    DF dk = from_int<DF>(k);
    term0 = div(fputil::quick_mul(term0, mt), fputil::quick_mul(dk, dk));
    term1 = div(fputil::quick_mul(term1, mt),
                fputil::quick_mul(dk, from_int<DF>(k + 1)));
    r.j0 = fputil::quick_add(r.j0, term0);
    r.j1 = fputil::quick_add(r.j1, term1);
    if (with_y) {
      harmonic = harmonic_next;
      harmonic_next =
          fputil::quick_add(harmonic, div(DF(1.0), from_int<DF>(k + 1)));
      r.s0 = fputil::quick_sub(r.s0, fputil::quick_mul(term0, harmonic));
      r.s1 = fputil::quick_add(
          r.s1,
          fputil::quick_mul(term1, fputil::quick_add(harmonic, harmonic_next)));
    }
    // Once k exceeds x the terms fall by more than a factor of four.
    if (static_cast<double>(k) > x &&
        (term0.mantissa.is_zero() || magnitude(term0) < floor) &&
        (term1.mantissa.is_zero() || magnitude(term1) < floor))
      break;
  }
  return r;
}

// Past 8 the terms reach 2^9 and more, which leaves too few of 128 bits for
// the values near the zeros, where they are small; there the sums are taken
// in 256 bits.
LIBC_INLINE Series series(double x, bool with_y) {
  if (x <= 8.0) {
    SeriesOf<DFloat128> r = series_sum<DFloat128>(x, with_y, -140);
    return {r.j0, r.j1, r.s0, r.s1};
  }
  SeriesOf<DFloat256> r = series_sum<DFloat256>(x, with_y, -200);
  return {narrow(r.j0), narrow(r.j1), narrow(r.s0), narrow(r.s1)};
}

// sin(x - m pi/128) and cos(x - m pi/128) for x >= 1, reduced the way the
// accurate pass of sin reduces its argument.
LIBC_INLINE void sincos_shifted(double x, unsigned m, DFloat128 &s,
                                DFloat128 &c) {
  using namespace range_reduction_double_internal;
  using FPBits = fputil::FPBits<double>;

  DoubleDouble y;
  unsigned k;
  DFloat128 u;
  LargeRangeReduction large{};
  if (FPBits(x).get_biased_exponent() < FPBits::EXP_BIAS + FAST_PASS_EXPONENT) {
    k = range_reduction_small(x, y);
    u = range_reduction_small_f128(x);
  } else {
    k = large.fast(x, y);
    u = large.accurate();
  }
  DFloat128 sin_u, cos_u;
  sincos_eval_internal::sincos_eval(u, sin_u, cos_u);

  auto sin_k = [](unsigned kk) -> DFloat128 {
    unsigned idx = (kk & 64) ? 64 - (kk & 63) : (kk & 63);
    DFloat128 ans = SIN_K_PI_OVER_128_F128[idx];
    if (kk & 128)
      ans.sign = Sign::NEG;
    return ans;
  };
  k -= m;
  DFloat128 sk = sin_k(k), ck = sin_k(k + 64);
  s = fputil::quick_add(fputil::quick_mul(sk, cos_u),
                        fputil::quick_mul(ck, sin_u));
  c = fputil::quick_sub(fputil::quick_mul(ck, cos_u),
                        fputil::quick_mul(sk, sin_u));
}

// J_nu(x) and Y_nu(x) for nu = 0 or 1 and x >= 32 from Hankel's expansions:
//
//   J_nu(x) = sqrt(2/(pi x)) (P cos chi - Q sin chi)
//   Y_nu(x) = sqrt(2/(pi x)) (P sin chi + Q cos chi)
//
// with chi = x - (2 nu + 1) pi/4, a_0 = 1,
// a_k = a_{k-1} (4 nu^2 - (2k - 1)^2) / (8k), and
//
//   P = sum (-1)^k a_{2k} / x^{2k},  Q = sum (-1)^k a_{2k+1} / x^{2k+1}.
LIBC_INLINE void hankel(double x, unsigned nu, DFloat128 &j, DFloat128 &y) {
  DFloat128 w = div(DFloat128(1.0), DFloat128(x));
  DFloat128 mu = from_int(4 * nu * nu);
  DFloat128 a(1.0), p(1.0), q{};
  int previous = 1;
  for (unsigned k = 1;; ++k) {
    unsigned odd = 2 * k - 1;
    DFloat128 f = fputil::quick_sub(mu, from_int(odd * odd));
    a = div(fputil::quick_mul(fputil::quick_mul(a, f), w), from_int(8 * k));
    if (a.mantissa.is_zero())
      break;
    int mag = magnitude(a);
    // The series is asymptotic: stop at its smallest term, or once the
    // terms no longer matter.
    if (mag < -130 || (k > 2 && mag > previous))
      break;
    previous = mag;
    DFloat128 term = a;
    if ((k / 2) & 1)
      term.sign = term.sign.negate();
    if (k & 1)
      q = fputil::quick_add(q, term);
    else
      p = fputil::quick_add(p, term);
  }
  DFloat128 s, c;
  sincos_shifted(x, nu ? 96 : 32, s, c);
  DFloat128 amp = sqrt_dyadic(fputil::quick_mul(TWO_OVER_PI, w));
  j = fputil::quick_mul(
      amp, fputil::quick_sub(fputil::quick_mul(p, c), fputil::quick_mul(q, s)));
  y = fputil::quick_mul(
      amp, fputil::quick_add(fputil::quick_mul(p, s), fputil::quick_mul(q, c)));
}

// J0, J1, Y0 and Y1 at x > 0. The Y values are only computed when asked for.
struct Values {
  DFloat128 j0, j1, y0, y1;
};

LIBC_INLINE Values values(double x, bool with_y) {
  Values v{};
  if (x >= SERIES_LIMIT) {
    hankel(x, 0, v.j0, v.y0);
    hankel(x, 1, v.j1, v.y1);
    return v;
  }
  Series s = series(x, with_y);
  v.j0 = s.j0;
  v.j1 = s.j1;
  if (with_y) {
    DFloat128 xd(x);
    DFloat128 lg =
        fputil::quick_add(log_dyadic(fputil::mul_pow_2(xd, -1)), EULER);
    v.y0 = fputil::quick_mul(
        TWO_OVER_PI, fputil::quick_add(fputil::quick_mul(lg, s.j0), s.s0));
    DFloat128 y1 = fputil::quick_mul(TWO_OVER_PI, fputil::quick_mul(lg, s.j1));
    y1 = fputil::quick_sub(y1, fputil::quick_mul(ONE_OVER_PI, s.s1));
    v.y1 = fputil::quick_sub(y1, div(TWO_OVER_PI, xd));
  }
  return v;
}

// Handles NaN and infinity for the J functions. Returns true with the
// result in *r when x is one of them.
LIBC_INLINE bool j_special(double x, double &r) {
  using FPBits = fputil::FPBits<double>;
  FPBits xbits(x);
  if (LIBC_LIKELY(xbits.is_finite()))
    return false;
  if (xbits.is_nan()) {
    if (xbits.is_signaling_nan()) {
      fputil::raise_except_if_required(FE_INVALID);
      r = FPBits::quiet_nan().get_val();
    } else {
      r = x;
    }
    return true;
  }
  r = 0.0;
  return true;
}

// Handles NaN, infinity, zero and negative x for the Y functions, which are
// defined for x > 0 only.
LIBC_INLINE bool y_special(double x, double &r) {
  using FPBits = fputil::FPBits<double>;
  FPBits xbits(x);
  if (xbits.is_nan()) {
    if (xbits.is_signaling_nan()) {
      fputil::raise_except_if_required(FE_INVALID);
      r = FPBits::quiet_nan().get_val();
    } else {
      r = x;
    }
    return true;
  }
  if (xbits.is_zero()) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_DIVBYZERO);
    r = FPBits::inf(Sign::NEG).get_val();
    return true;
  }
  if (xbits.is_neg()) {
    fputil::set_errno_if_required(EDOM);
    fputil::raise_except_if_required(FE_INVALID);
    r = FPBits::quiet_nan().get_val();
    return true;
  }
  if (xbits.is_inf()) {
    r = 0.0;
    return true;
  }
  return false;
}

// Rounds to double, setting errno where that overflows.
LIBC_INLINE double to_double(const DFloat128 &v) {
  double r = static_cast<double>(v);
  if (LIBC_UNLIKELY(fputil::FPBits<double>(r).is_inf()))
    fputil::set_errno_if_required(ERANGE);
  return r;
}

LIBC_INLINE DFloat128 negate_if(DFloat128 v, bool neg) {
  if (neg)
    v.sign = v.sign.negate();
  return v;
}

} // namespace bessel_internal

LIBC_INLINE double j0(double x) {
  using namespace bessel_internal;
  double r;
  if (j_special(x, r))
    return r;
  double ax = __builtin_fabs(x);
  if (ax == 0.0)
    return 1.0;
  return to_double(values(ax, false).j0);
}

LIBC_INLINE double j1(double x) {
  using namespace bessel_internal;
  double r;
  if (j_special(x, r))
    return x < 0.0 ? -r : r;
  if (x == 0.0)
    return x;
  double ax = __builtin_fabs(x);
  return to_double(negate_if(values(ax, false).j1, x < 0.0));
}

LIBC_INLINE double y0(double x) {
  using namespace bessel_internal;
  double r;
  if (y_special(x, r))
    return r;
  return to_double(values(x, true).y0);
}

LIBC_INLINE double y1(double x) {
  using namespace bessel_internal;
  double r;
  if (y_special(x, r))
    return r;
  return to_double(values(x, true).y1);
}

// J_n(-x) = (-1)^n J_n(x) and J_{-n}(x) = (-1)^n J_n(x). Below x the
// forward recurrence J_{k+1} = (2k/x) J_k - J_{k-1} is stable; from x up it
// is run backwards from well past n, as Miller did, and the result scaled
// by whichever of J0 and J1 is further from a zero.
LIBC_INLINE double jn(int n, double x) {
  using namespace bessel_internal;
  unsigned un =
      n < 0 ? 0u - static_cast<unsigned>(n) : static_cast<unsigned>(n);
  bool neg = (un & 1) && ((n < 0) != (x < 0.0));
  double r;
  if (j_special(x, r))
    return neg ? -r : r;
  if (un == 0)
    return j0(x);
  double ax = __builtin_fabs(x);
  if (ax == 0.0)
    return neg ? -0.0 : 0.0;
  if (un == 1)
    return to_double(negate_if(values(ax, false).j1, neg));

  double dn = static_cast<double>(un);
  if (dn >= ax) {
    // J_n(x) <= (e x / 2n)^n, so past this bound it is below 2^-1100.
    double ratio = 2.718281828459045 * ax / (2.0 * dn);
    if (ratio < 0.5 &&
        dn * static_cast<double>(log2_dyadic_long(ratio)) < -1100.0) {
      fputil::raise_except_if_required(FE_UNDERFLOW | FE_INEXACT);
      fputil::set_errno_if_required(ERANGE);
      return neg ? -0.0 : 0.0;
    }
  }

  DFloat128 xd(ax);
  DFloat128 two_over_x = div(DFloat128(2.0), xd);
  Values v = values(ax, false);
  DFloat128 result;
  if (dn < ax) {
    DFloat128 prev = v.j0, cur = v.j1;
    for (unsigned k = 1; k < un; ++k) {
      DFloat128 next = fputil::quick_sub(
          fputil::quick_mul(fputil::quick_mul(from_int(k), two_over_x), cur),
          prev);
      prev = cur;
      cur = next;
    }
    result = cur;
  } else {
    unsigned start = un + 64 +
                     static_cast<unsigned>(
                         fputil::sqrt<double>(40.0 * static_cast<double>(un)));
    DFloat128 next{}, cur(1.0), at_n{};
    for (unsigned k = start; k > 0; --k) {
      if (k == un)
        at_n = cur;
      // Keep the values inside the exponent range by rescaling them all.
      if (magnitude(cur) > 1 << 20) {
        int shift = -magnitude(cur);
        cur = fputil::mul_pow_2(cur, shift);
        next = fputil::mul_pow_2(next, shift);
        if (!at_n.mantissa.is_zero())
          at_n = fputil::mul_pow_2(at_n, shift);
      }
      DFloat128 prev = fputil::quick_sub(
          fputil::quick_mul(fputil::quick_mul(from_int(k), two_over_x), cur),
          next);
      next = cur;
      cur = prev;
    }
    // cur and next now stand for J0 and J1, up to the same factor.
    bool use_j0 = magnitude(v.j0) >= magnitude(v.j1);
    DFloat128 scale = use_j0 ? div(v.j0, cur) : div(v.j1, next);
    result = fputil::quick_mul(at_n, scale);
  }
  return to_double(negate_if(result, neg));
}

// Y_{-n}(x) = (-1)^n Y_n(x), and the forward recurrence is stable for Y.
LIBC_INLINE double yn(int n, double x) {
  using namespace bessel_internal;
  unsigned un =
      n < 0 ? 0u - static_cast<unsigned>(n) : static_cast<unsigned>(n);
  bool neg = (un & 1) && n < 0;
  double r;
  if (y_special(x, r))
    return (neg && !fputil::FPBits<double>(r).is_nan()) ? -r : r;
  Values v = values(x, true);
  if (un == 0)
    return to_double(v.y0);
  DFloat128 two_over_x = div(DFloat128(2.0), DFloat128(x));
  DFloat128 prev = v.y0, cur = v.y1;
  for (unsigned k = 1; k < un; ++k) {
    // Past 2^1100 the result can only overflow.
    if (magnitude(cur) > 1100)
      break;
    DFloat128 next = fputil::quick_sub(
        fputil::quick_mul(fputil::quick_mul(from_int(k), two_over_x), cur),
        prev);
    prev = cur;
    cur = next;
  }
  return to_double(negate_if(cur, neg));
}

} // namespace math
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_BESSEL_H

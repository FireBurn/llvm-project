//===-- Implementation of powl --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/math/powl.h"

#include "hdr/fenv_macros.h"
#include "src/__support/CPP/bit.h"
#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/FPUtil/dyadic_float.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"
#include "src/__support/math/log2l_impl.h"
#include "src/__support/math/powl_impl.h"
#include "src/__support/uint128.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

using FPBits = fputil::FPBits<long double>;

// Whether y is a whole number, and if so whether it is an odd one. Which of
// the two matters only for a negative base: x^y is real there just when y is
// a whole number, and its sign follows from whether that number is odd.
enum class Parity { NOT_INTEGER, EVEN, ODD };

LIBC_INLINE Parity parity_of(long double y) {
  FPBits ybits(y);
  int exponent = ybits.get_explicit_exponent();
  // Below one in size, so a whole number only if it is zero, which the
  // caller has already dealt with.
  if (exponent < 0)
    return Parity::NOT_INTEGER;
  // Wider than the mantissa, so every representable value is a whole number
  // and all of them are even.
  constexpr int MANTISSA_BITS = FPBits::FRACTION_LEN;
  if (exponent >= MANTISSA_BITS)
    return Parity::EVEN;

  // The bits below the point have to be clear for this to be a whole number.
  using StorageType = FPBits::StorageType;
  StorageType mantissa = ybits.get_explicit_mantissa();
  StorageType fractional_mask =
      (StorageType(1) << (MANTISSA_BITS - exponent)) - 1;
  if ((mantissa & fractional_mask) != 0)
    return Parity::NOT_INTEGER;

  StorageType unit = StorageType(1) << (MANTISSA_BITS - exponent);
  return (mantissa & unit) != 0 ? Parity::ODD : Parity::EVEN;
}


// An odd number and the power of two it is scaled by.
struct Dyadic {
  uint64_t odd;
  int exponent;
};

LIBC_INLINE Dyadic dyadic_of(long double v) {
  FPBits bits(v);
  uint64_t m = static_cast<uint64_t>(bits.get_explicit_mantissa());
  int shift = cpp::countr_zero(m);
  return {m >> shift,
          bits.get_explicit_exponent() - FPBits::FRACTION_LEN + shift};
}

// b^n if it is below 2^65, which is as wide as an exact result or a point
// half way between two long doubles can be.
LIBC_INLINE bool small_power(uint64_t b, uint64_t n, UInt128 &result) {
  // b >= 2^(width - 1), so this bounds the power from below. Once it passes,
  // the power is also known to fit in 128 bits.
  uint64_t width = static_cast<uint64_t>(cpp::bit_width(b));
  if (n > 64 || (width - 1) * n > 64)
    return false;
  UInt128 p = 1;
  for (uint64_t i = 0; i < n; ++i)
    p *= b;
  if ((p >> 65) != 0)
    return false;
  result = p;
  return true;
}

LIBC_INLINE bool exact_sqrt(uint64_t v, uint64_t &root) {
  uint64_t s = static_cast<uint64_t>(__builtin_sqrt(static_cast<double>(v)));
  for (uint64_t c = (s > 0 ? s - 1 : 0); c <= s + 1; ++c) {
    if (static_cast<UInt128>(c) * c == v) {
      root = c;
      return true;
    }
  }
  return false;
}

// x^y for a positive x, when it is exact in a hundred and twenty eight bits
// and not a power of two. These are the results the approximation cannot
// round: one that is a long double, or half way between two, may come out
// just either side of where it is. Such results are rational, which needs y
// to be a whole number, or an odd multiple of 2^-k with x a 2^k-th power.
// A negative y gives the reciprocal of an odd number, which is not a finite
// binary fraction.
LIBC_INLINE bool exact_powl(long double x, long double y,
                            fputil::DyadicFloat<128> &result) {
  if (FPBits(y).is_neg())
    return false;
  Dyadic dx = dyadic_of(x);
  Dyadic dy = dyadic_of(y);
  if (dx.odd == 1)
    return false;

  uint64_t base = dx.odd;
  uint64_t n;
  int exponent;
  if (dy.exponent >= 0) {
    if (dy.exponent > 6)
      return false;
    n = dy.odd << dy.exponent;
    exponent = dx.exponent * static_cast<int>(n);
  } else {
    int k = -dy.exponent;
    // 3^32 is already wider than 64 bits.
    if (k > 5 || (dx.exponent & ((1 << k) - 1)) != 0)
      return false;
    for (int i = 0; i < k; ++i)
      if (!exact_sqrt(base, base))
        return false;
    n = dy.odd;
    exponent = (dx.exponent >> k) * static_cast<int>(n);
  }

  UInt128 odd;
  if (!small_power(base, n, odd))
    return false;
  result = fputil::DyadicFloat<128>(
      Sign::POS, exponent, fputil::DyadicFloat<128>::MantissaType(odd));
  return true;
}

} // namespace

LLVM_LIBC_FUNCTION(long double, powl, (long double x, long double y)) {
  using DFloat128 = fputil::DyadicFloat<128>;

  FPBits xbits(x), ybits(y);

  // Anything to the power of zero is one, even a quiet NaN.
  if (ybits.is_zero())
    return 1.0L;

  // One to any power is one, even a quiet NaN.
  if (x == 1.0L)
    return 1.0L;

  if (xbits.is_signaling_nan() || ybits.is_signaling_nan()) {
    fputil::raise_except_if_required(FE_INVALID);
    return FPBits::quiet_nan().get_val();
  }
  if (xbits.is_nan())
    return x;
  if (ybits.is_nan())
    return y;

  const bool x_neg = xbits.is_neg();
  const bool y_neg = ybits.is_neg();
  const Parity parity = parity_of(y);

  // A negative base raised to anything but a whole number is not real.
  if (x_neg && parity == Parity::NOT_INTEGER && !xbits.is_zero()) {
    fputil::set_errno_if_required(EDOM);
    fputil::raise_except_if_required(FE_INVALID);
    return FPBits::quiet_nan().get_val();
  }

  if (ybits.is_inf()) {
    long double ax = xbits.abs().get_val();
    if (ax == 1.0L)
      return 1.0L; // including (-1)^inf
    // Greater than one grows without bound for a positive exponent, and
    // shrinks to zero for a negative one. Less than one does the reverse.
    return ((ax > 1.0L) == !y_neg) ? FPBits::inf().get_val() : 0.0L;
  }

  if (xbits.is_inf()) {
    bool negate = x_neg && parity == Parity::ODD;
    if (y_neg)
      return negate ? -0.0L : 0.0L;
    return negate ? FPBits::inf(Sign::NEG).get_val() : FPBits::inf().get_val();
  }

  if (xbits.is_zero()) {
    bool negate = x_neg && parity == Parity::ODD;
    if (y_neg) {
      fputil::set_errno_if_required(ERANGE);
      fputil::raise_except_if_required(FE_DIVBYZERO);
      return negate ? FPBits::inf(Sign::NEG).get_val()
                    : FPBits::inf().get_val();
    }
    return negate ? -0.0L : 0.0L;
  }

  // x^y = 2^(y * log2(|x|)), with the sign put back afterwards. Both halves
  // are carried at a hundred and twenty eight bits, which leaves room above
  // the sixty four the result shows: the multiply is where an error would be
  // magnified, since an absolute error there becomes a relative one here.
  // Given to the result before it is rounded, since in the directed modes
  // the sign decides which way that goes.
  const Sign result_sign =
      (x_neg && parity == Parity::ODD) ? Sign::NEG : Sign::POS;

  DFloat128 log2_x = math::log2_dyadic_long(xbits.abs().get_val());
  DFloat128 prod = fputil::quick_mul(DFloat128(y), log2_x);

  // Out of range before the exponent is even formed.
  double prod_d = static_cast<double>(prod);
  if (LIBC_UNLIKELY(prod_d > 16384.0)) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_OVERFLOW);
    return static_cast<long double>(DFloat128(
        result_sign, 2 * FPBits::EXP_BIAS, DFloat128::MantissaType(1)));
  }
  if (LIBC_UNLIKELY(prod_d < -16446.0)) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_UNDERFLOW);
    return static_cast<long double>(DFloat128(
        result_sign, -2 * FPBits::EXP_BIAS, DFloat128::MantissaType(1)));
  }

  DFloat128 result;
  if (!exact_powl(xbits.abs().get_val(), y, result))
    result = math::exp2_dyadic(prod);
  result.sign = result_sign;
  long double r = static_cast<long double>(result);

  FPBits rbits(r);
  if (LIBC_UNLIKELY(rbits.is_inf())) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_OVERFLOW);
  } else if (LIBC_UNLIKELY(rbits.is_zero())) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_UNDERFLOW);
  }

  return r;
}

} // namespace LIBC_NAMESPACE_DECL

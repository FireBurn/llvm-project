//===-- Implementation of powl --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/math/powl.h"

#include "hdr/fenv_macros.h"
#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/FPUtil/dyadic_float.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"
#include "src/__support/math/log2l_impl.h"
#include "src/__support/math/powl_impl.h"

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
  const bool negate_result = x_neg && parity == Parity::ODD;

  DFloat128 log2_x = math::log2_dyadic_long(xbits.abs().get_val());
  DFloat128 prod = fputil::quick_mul(DFloat128(y), log2_x);

  // Out of range before the exponent is even formed.
  double prod_d = static_cast<double>(prod);
  if (LIBC_UNLIKELY(prod_d > 16384.0)) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_OVERFLOW);
    return negate_result ? FPBits::inf(Sign::NEG).get_val()
                         : FPBits::inf().get_val();
  }
  if (LIBC_UNLIKELY(prod_d < -16446.0)) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_UNDERFLOW);
    return negate_result ? -0.0L : 0.0L;
  }

  DFloat128 result = math::exp2_dyadic(prod);
  long double r = static_cast<long double>(result);

  FPBits rbits(r);
  if (LIBC_UNLIKELY(rbits.is_inf())) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_OVERFLOW);
  } else if (LIBC_UNLIKELY(rbits.is_zero())) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_UNDERFLOW);
  }

  return negate_result ? -r : r;
}

} // namespace LIBC_NAMESPACE_DECL

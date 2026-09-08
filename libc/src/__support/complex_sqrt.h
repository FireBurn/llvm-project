//===-- The square root of a complex number ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_COMPLEX_SQRT_H
#define LLVM_LIBC_SRC___SUPPORT_COMPLEX_SQRT_H

#include "src/__support/CPP/bit.h"
#include "src/__support/FPUtil/BasicOperations.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/FPUtil/ManipulationFunctions.h"
#include "src/__support/FPUtil/multiply_add.h"
#include "src/__support/FPUtil/sqrt.h"
#include "src/__support/complex_type.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

// The principal square root: the one whose real part is not negative. It is
// taken from the modulus rather than from the halved argument, since that
// stays accurate where the number lies close to the axes.
//
//   for a >= 0:  sqrt(a + bi) = t + (b / 2t)i,  t = sqrt((|z| + a) / 2)
//   for a <  0:  sqrt(a + bi) = (|b| / 2t) + (copysign(t, b))i,
//                               t = sqrt((|z| - a) / 2)
//
// Each form adds two numbers of the same sign, so neither loses the digits a
// subtraction of two close numbers would.
template <typename T> LIBC_INLINE Complex<T> complex_sqrt(Complex<T> z) {
  using FPBits = fputil::FPBits<T>;

  T a = z.real;
  T b = z.imag;

  const FPBits a_bits(a);
  const FPBits b_bits(b);

  // The answer for an infinity is fixed whatever the other part is, even
  // where that other part is not a number, so these come before the checks
  // for one.
  if (b_bits.is_inf())
    return {FPBits::inf().get_val(), b};

  if (a_bits.is_inf()) {
    if (a_bits.is_neg())
      return {b_bits.is_nan() ? b : T(0.0),
              fputil::copysign(FPBits::inf().get_val(), b)};
    return {FPBits::inf().get_val(),
            b_bits.is_nan() ? b : fputil::copysign(T(0.0), b)};
  }

  if (a_bits.is_nan() || b_bits.is_nan()) {
    const T nan = FPBits::quiet_nan().get_val();
    return {nan, nan};
  }

  if (a_bits.is_zero() && b_bits.is_zero())
    return {T(0.0), b};

  // Both parts are scaled by the same power of four so that the modulus can
  // be taken without overflowing or falling to zero. A power of four halves
  // to a power of two, which the square root then undoes exactly.
  int scale = 0;
  const T abs_a = fputil::abs(a);
  const T abs_b = fputil::abs(b);
  const T largest = abs_a > abs_b ? abs_a : abs_b;

  // The band the modulus is taken in: far enough below the largest finite
  // number that squaring cannot overflow, and far enough above the smallest
  // normal one that squaring cannot fall to zero.
  constexpr int MAX_EXP = FPBits::MAX_BIASED_EXPONENT - FPBits::EXP_BIAS - 1;
  constexpr int MIN_EXP = 1 - FPBits::EXP_BIAS;
  constexpr int HIGH = MAX_EXP - 4;
  constexpr int LOW = MIN_EXP + FPBits::FRACTION_LEN + 4;
  const int largest_exp = FPBits(largest).get_exponent();

  if (largest_exp > HIGH)
    scale = -(((largest_exp - HIGH) / 2 + 1) * 2);
  else if (largest_exp < LOW)
    scale = ((LOW - largest_exp) / 2 + 1) * 2;

  if (scale != 0) {
    a = fputil::ldexp(a, scale);
    b = fputil::ldexp(b, scale);
  }

  // The scaling above leaves both squares inside the range, so the modulus
  // is taken directly rather than through hypot, which is not available for
  // every type this is instantiated with. What each squaring and the sum
  // round away is recovered and added back before the root is taken.
  const T aa = a * a;
  const T bb = b * b;
  const T high = aa > bb ? aa : bb;
  const T low = aa > bb ? bb : aa;
  const T sum = high + low;
  const T lost = ((high - sum) + low) + fputil::multiply_add(a, a, -aa) +
                 fputil::multiply_add(b, b, -bb);
  const T modulus = fputil::sqrt<T>(sum + lost);
  T real;
  T imag;
  if (a >= T(0.0)) {
    real = fputil::sqrt<T>((modulus + a) * T(0.5));
    imag = b / (real + real);
  } else {
    imag = fputil::sqrt<T>((modulus - a) * T(0.5));
    real = fputil::abs(b) / (imag + imag);
    imag = fputil::copysign(imag, b);
  }

  // Both parts were scaled by 4^n, so both roots are scaled by 2^n.
  if (scale != 0) {
    real = fputil::ldexp(real, -scale / 2);
    imag = fputil::ldexp(imag, -scale / 2);
  }
  return {real, imag};
}

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_COMPLEX_SQRT_H

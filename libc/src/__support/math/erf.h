//===-- Implementation of the double precision error function ---*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_MATH_ERF_H
#define LLVM_LIBC_SRC___SUPPORT_MATH_ERF_H

#include "erf_common.h"
#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"

namespace LIBC_NAMESPACE_DECL {

namespace math {

LIBC_INLINE static double erf(double x) {
  using FPBits = fputil::FPBits<double>;
  using namespace erf_internal;

  FPBits xbits(x);
  const Sign sign = xbits.sign();
  xbits.set_sign(Sign::POS);
  const double ax = xbits.get_val();

  if (LIBC_UNLIKELY(xbits.is_inf_or_nan())) {
    if (xbits.is_nan())
      return x + x;
    // erf runs up to one either way out.
    return sign.is_neg() ? -1.0 : 1.0;
  }

  const auto with_sign = [sign](double value) {
    return sign.is_neg() ? -value : value;
  };

  if (ax <= 1.0)
    return erf_small(x);

  // Past six the answer is nearer one than anything else, though it never
  // reaches it, which is what the inexact flag says.
  if (ax >= 6.0) {
    fputil::raise_except_if_required(FE_INEXACT);
    return with_sign(1.0);
  }

  // In between the answer is near one, so what is worked out is how far
  // short of it the answer falls.
  return with_sign(1.0 - erfc_positive(ax));
}

} // namespace math

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_ERF_H

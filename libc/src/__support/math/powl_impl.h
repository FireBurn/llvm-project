//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// pow of long doubles to full precision
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_MATH_POWL_IMPL_H
#define LLVM_LIBC_SRC___SUPPORT_MATH_POWL_IMPL_H

#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/FPUtil/dyadic_float.h"
#include "src/__support/macros/config.h"
#include "src/__support/math/exp2_accurate_128.h"
#include "src/__support/sign.h"
#include "src/__support/uint128.h"

namespace LIBC_NAMESPACE_DECL {
namespace math {

// 2^y for a y carried in more precision than a double, which is what the
// result of y * log2(x) needs to be if the answer is to be good to the
// sixty four bits a long double shows.
//
// y is split into the nearest integer n and a fraction f with |f| <= 1/2.
// The fraction goes to the 128 bit routine the double exp2 falls back on,
// and n only moves the exponent.
LIBC_INLINE fputil::DyadicFloat<128>
exp2_dyadic(const fputil::DyadicFloat<128> &y) {
  using DFloat128 = fputil::DyadicFloat<128>;

  double y_hi = static_cast<double>(y);
  int n = static_cast<int>(y_hi < 0 ? y_hi - 0.5 : y_hi + 0.5);
  DFloat128 f = fputil::quick_sub(y, DFloat128(static_cast<double>(n)));

  DFloat128 r;
  if (f.mantissa.is_zero()) {
    r = DFloat128(1.0);
  } else if (f.exponent < -181) {
    // Below 2^-54 the routine's range reduction no longer applies. Here
    // 2^f = 1 + f * ln 2 is good to 2^-108 and more.
    DFloat128 ln2(Sign::POS, -128,
                  UInt<128>({0xc9e3b39803f2f6afULL, 0xb17217f7d1cf79abULL}));
    r = fputil::quick_add(DFloat128(1.0), fputil::quick_mul(f, ln2));
  } else {
    r = double_eval::exp2_df128(f);
  }
  r.exponent += n;
  return r;
}

} // namespace math
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_POWL_IMPL_H

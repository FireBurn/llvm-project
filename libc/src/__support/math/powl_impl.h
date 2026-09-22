//===-- pow of long doubles to full precision -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_MATH_POWL_IMPL_H
#define LLVM_LIBC_SRC___SUPPORT_MATH_POWL_IMPL_H

#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/FPUtil/dyadic_float.h"
#include "src/__support/macros/config.h"
#include "src/__support/math/common_constants.h"
#include "src/__support/math/exp2.h"

namespace LIBC_NAMESPACE_DECL {
namespace math {

// 2^y for a y carried in more precision than a double, which is what the
// result of y * log2(x) needs to be if the answer is to be good to the
// sixty four bits a long double shows.
//
// The reduction is the one the double routine uses: 2^y is split into a
// power of two, two table entries, and 2^dx for a small dx. Only dx is
// carried at full width here, since that is where the extra bits of y end
// up once the rest has been taken out.
LIBC_INLINE fputil::DyadicFloat<128>
exp2_dyadic(const fputil::DyadicFloat<128> &y) {
  using namespace exp2_internal;
  using namespace common_constants_internal;

  double y_hi = static_cast<double>(y);

  // k is round(y * 2^12), taken the way the double routine takes it.
  int k = static_cast<int>(
      cpp::bit_cast<uint64_t>(y_hi + 0x1.8000'0000'4p21) >> 19);
  double kd = static_cast<double>(k);

  uint32_t idx1 = static_cast<uint32_t>((k >> 6) & 0x3f);
  uint32_t idx2 = static_cast<uint32_t>(k & 0x3f);
  int hi = k >> 12;

  // dx = y - k * 2^-12, with the whole of y rather than its leading double.
  DFloat128 dx = fputil::quick_sub(y, DFloat128(kd * 0x1.0p-12));

  DFloat128 exp_mid1 =
      fputil::quick_add(DFloat128(EXP2_MID1[idx1].hi),
                        fputil::quick_add(DFloat128(EXP2_MID1[idx1].mid),
                                          DFloat128(EXP2_MID1[idx1].lo)));
  DFloat128 exp_mid2 =
      fputil::quick_add(DFloat128(EXP2_MID2[idx2].hi),
                        fputil::quick_add(DFloat128(EXP2_MID2[idx2].mid),
                                          DFloat128(EXP2_MID2[idx2].lo)));

  DFloat128 r = fputil::quick_mul(fputil::quick_mul(exp_mid1, exp_mid2),
                                  poly_approx_f128(dx));
  r.exponent += hi;
  return r;
}

} // namespace math
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_POWL_IMPL_H

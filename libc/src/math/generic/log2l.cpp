//===-- Implementation of log2l -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/math/log2l.h"

#include "hdr/fenv_macros.h"
#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"
#include "src/__support/math/log2l_impl.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(long double, log2l, (long double x)) {
  using FPBits = fputil::FPBits<long double>;
  FPBits xbits(x);

  if (LIBC_UNLIKELY(xbits.is_signaling_nan())) {
    fputil::raise_except_if_required(FE_INVALID);
    return FPBits::quiet_nan().get_val();
  }
  if (xbits.is_nan())
    return x;

  if (xbits.is_zero()) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_DIVBYZERO);
    return FPBits::inf(Sign::NEG).get_val();
  }
  if (xbits.is_neg()) {
    fputil::set_errno_if_required(EDOM);
    fputil::raise_except_if_required(FE_INVALID);
    return FPBits::quiet_nan().get_val();
  }
  if (xbits.is_inf())
    return x;

  if (x == 1.0L)
    return 0.0L;

  return static_cast<long double>(math::log2_dyadic_long(x));
}

} // namespace LIBC_NAMESPACE_DECL

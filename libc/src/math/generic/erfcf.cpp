//===-- Implementation of erfcf function ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/math/erfcf.h"
#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"
#include "src/__support/math/erfc.h"

namespace LIBC_NAMESPACE_DECL {

// The double result has 29 bits to spare, so rounding it to float is off only
// where it lies within that of a halfway point between two floats.
LLVM_LIBC_FUNCTION(float, erfcf, (float x)) {
  double r = math::erfc(static_cast<double>(x));
  float f = static_cast<float>(r);
  if (LIBC_UNLIKELY(r != 0.0 && fputil::FPBits<float>(f).is_zero()))
    fputil::set_errno_if_required(ERANGE);
  return f;
}

} // namespace LIBC_NAMESPACE_DECL

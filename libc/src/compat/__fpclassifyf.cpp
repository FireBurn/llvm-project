//===-- Implementation of __fpclassifyf -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/compat/__fpclassifyf.h"

#include "hdr/math_macros.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// glibc puts this behind fpclassify for float. Here fpclassify is a macro
// over the compiler builtin, so nothing else defines the name, and code
// already built against glibc asks for it.
LLVM_LIBC_FUNCTION(int, __fpclassifyf, (float x)) {
  fputil::FPBits<float> bits(x);
  if (bits.is_nan())
    return FP_NAN;
  if (bits.is_inf())
    return FP_INFINITE;
  if (bits.is_zero())
    return FP_ZERO;
  if (bits.is_subnormal())
    return FP_SUBNORMAL;
  return FP_NORMAL;
}

} // namespace LIBC_NAMESPACE_DECL

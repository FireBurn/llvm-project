//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the implementation of csqrtf.
///
//===----------------------------------------------------------------------===//

#include "src/complex/csqrtf.h"
#include "src/__support/CPP/bit.h"
#include "src/__support/common.h"
#include "src/__support/complex_sqrt.h"
#include "src/__support/complex_type.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(_Complex float, csqrtf, (_Complex float x)) {
  Complex<float> result =
      internal::complex_sqrt(cpp::bit_cast<Complex<float>>(x));
  return cpp::bit_cast<_Complex float>(result);
}

} // namespace LIBC_NAMESPACE_DECL

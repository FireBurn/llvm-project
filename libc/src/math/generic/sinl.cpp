//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Long double sin function
///
//===----------------------------------------------------------------------===//

#include "src/math/sinl.h"
#include "src/__support/FPUtil/x86_64/x87_trig.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(long double, sinl, (long double x)) {
  long double result;
  if (fputil::x87::special(x, result))
    return result;
  return fputil::x87::sin(x);
}

} // namespace LIBC_NAMESPACE_DECL

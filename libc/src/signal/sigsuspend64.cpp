//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of sigsuspend64.
///
//===----------------------------------------------------------------------===//

#include "src/signal/sigsuspend64.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/signal/sigsuspend.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, sigsuspend64, (const sigset64_t *set)) {
  return sigsuspend(set);
}

} // namespace LIBC_NAMESPACE_DECL

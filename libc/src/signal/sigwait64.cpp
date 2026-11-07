//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of sigwait64.
///
//===----------------------------------------------------------------------===//

#include "src/signal/sigwait64.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/signal/sigwait.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, sigwait64,
                   (const sigset64_t *__restrict set, int *__restrict sig)) {
  return sigwait(set, sig);
}

} // namespace LIBC_NAMESPACE_DECL

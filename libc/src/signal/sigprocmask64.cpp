//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of sigprocmask64.
///
//===----------------------------------------------------------------------===//

#include "src/signal/sigprocmask64.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/signal/sigprocmask.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, sigprocmask64,
                   (int how, const sigset64_t *__restrict set,
                    sigset64_t *__restrict oldset)) {
  return sigprocmask(how, set, oldset);
}

} // namespace LIBC_NAMESPACE_DECL

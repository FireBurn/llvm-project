//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of pthread_sigmask64.
///
//===----------------------------------------------------------------------===//

#include "src/signal/pthread_sigmask64.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/signal/pthread_sigmask.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_sigmask64,
                   (int how, const sigset64_t *__restrict set,
                    sigset64_t *__restrict oldset)) {
  return pthread_sigmask(how, set, oldset);
}

} // namespace LIBC_NAMESPACE_DECL

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of sigaddset64.
///
//===----------------------------------------------------------------------===//

#include "src/signal/sigaddset64.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/signal/sigaddset.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, sigaddset64, (sigset64_t * set, int signum)) {
  return sigaddset(set, signum);
}

} // namespace LIBC_NAMESPACE_DECL

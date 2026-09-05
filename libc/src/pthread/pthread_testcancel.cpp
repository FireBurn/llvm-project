//===-- Implementation of pthread_testcancel ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_testcancel.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/cancel.h"

namespace LIBC_NAMESPACE_DECL {

// A cancellation point put where the caller wants one, for code that would
// otherwise run for a long time without reaching any of its own.
LLVM_LIBC_FUNCTION(void, pthread_testcancel, ()) { internal::cancel_point(); }

} // namespace LIBC_NAMESPACE_DECL

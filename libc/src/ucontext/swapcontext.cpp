//===-- Implementation of swapcontext -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/ucontext/swapcontext.h"

#include "include/llvm-libc-types/ucontext_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/ucontext/getcontext.h"
#include "src/ucontext/setcontext.h"

namespace LIBC_NAMESPACE_DECL {

// Saves where we are into |oucp| and goes to |ucp|. When something later
// comes back to |oucp|, this returns 0 from the caller's point of view.
//
// The saved context resumes just after the getcontext below, on this same
// stack frame, so the flag which tells the two paths apart is a local. It
// has to be volatile: it is written before the switch and read after a
// return which the compiler cannot see, so it must live in memory rather
// than in a register.
LLVM_LIBC_FUNCTION(int, swapcontext,
                   (ucontext_t * oucp, const ucontext_t *ucp)) {
  if (oucp == nullptr || ucp == nullptr) {
    libc_errno = EINVAL;
    return -1;
  }

  volatile bool resumed = false;

  if (LIBC_NAMESPACE::getcontext(oucp) != 0)
    return -1;

  if (resumed)
    return 0;
  resumed = true;

  return LIBC_NAMESPACE::setcontext(ucp);
}

} // namespace LIBC_NAMESPACE_DECL

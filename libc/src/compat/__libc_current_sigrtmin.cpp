//===-- Implementation of __libc_current_sigrtmin ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/compat/__libc_current_sigrtmin.h"

#include "hdr/signal_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Both glibc and musl keep the first few real time signals for themselves and
// so report this through a call rather than a constant. Nothing here reserves
// any, but code compiled against either of them calls this to find out.
LLVM_LIBC_FUNCTION(int, __libc_current_sigrtmin, (void)) { return SIGRTMIN; }

} // namespace LIBC_NAMESPACE_DECL

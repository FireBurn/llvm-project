//===-- Implementation of __libc_current_sigrtmax ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/compat/__libc_current_sigrtmax.h"

#include "hdr/signal_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// The companion to __libc_current_sigrtmin, reported the same way and for
// the same reason: where the range ends is a property of the library, not of
// the header the caller compiled against.
LLVM_LIBC_FUNCTION(int, __libc_current_sigrtmax, (void)) { return SIGRTMAX; }

} // namespace LIBC_NAMESPACE_DECL

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of sys_signame.
///
//===----------------------------------------------------------------------===//

#include "src/signal/sys_signame.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/signal/signal_text.h"

namespace LIBC_NAMESPACE_DECL {

// NSIG is 65 on Linux. The entries are listed by repetition because an array
// cannot be initialised from a pack.
#define LIBC_SIGNAL_TEXT(n) signal_text::name(n)
#define LIBC_SIGNAL_TEXT4(n)                                                   \
  LIBC_SIGNAL_TEXT(n), LIBC_SIGNAL_TEXT(n + 1), LIBC_SIGNAL_TEXT(n + 2),       \
      LIBC_SIGNAL_TEXT(n + 3)
#define LIBC_SIGNAL_TEXT16(n)                                                  \
  LIBC_SIGNAL_TEXT4(n), LIBC_SIGNAL_TEXT4(n + 4), LIBC_SIGNAL_TEXT4(n + 8),    \
      LIBC_SIGNAL_TEXT4(n + 12)

static_assert(NSIG == 65, "the table below lists 65 signals");

LLVM_LIBC_VARIABLE(const char *const, sys_signame)[NSIG] = {
    LIBC_SIGNAL_TEXT16(0), LIBC_SIGNAL_TEXT16(16), LIBC_SIGNAL_TEXT16(32),
    LIBC_SIGNAL_TEXT16(48), LIBC_SIGNAL_TEXT(64)};

#undef LIBC_SIGNAL_TEXT16
#undef LIBC_SIGNAL_TEXT4
#undef LIBC_SIGNAL_TEXT

} // namespace LIBC_NAMESPACE_DECL

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The descriptions of the signals, indexed by number.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SIGNAL_SYS_SIGLIST_H
#define LLVM_LIBC_SRC_SIGNAL_SYS_SIGLIST_H

#include "hdr/signal_macros.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

extern const char *const sys_siglist[NSIG];

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SIGNAL_SYS_SIGLIST_H

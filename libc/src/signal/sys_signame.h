//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The names of the signals without the SIG, indexed by number.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SIGNAL_SYS_SIGNAME_H
#define LLVM_LIBC_SRC_SIGNAL_SYS_SIGNAME_H

#include "hdr/signal_macros.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

extern const char *const sys_signame[NSIG];

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SIGNAL_SYS_SIGNAME_H

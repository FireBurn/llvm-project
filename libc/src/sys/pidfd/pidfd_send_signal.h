//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for pidfd_send_signal.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_PIDFD_PIDFD_SEND_SIGNAL_H
#define LLVM_LIBC_SRC_SYS_PIDFD_PIDFD_SEND_SIGNAL_H

#include "hdr/types/siginfo_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int pidfd_send_signal(int pidfd, int sig, siginfo_t *info, unsigned int flags);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_PIDFD_PIDFD_SEND_SIGNAL_H

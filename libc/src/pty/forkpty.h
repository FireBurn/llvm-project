//===-- Implementation header for forkpty -----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_PTY_FORKPTY_H
#define LLVM_LIBC_SRC_PTY_FORKPTY_H

#include "hdr/types/pid_t.h"
#include "hdr/types/struct_termios.h"
#include "hdr/types/struct_winsize.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Opens a pair of joined terminals and starts a child attached to the second
// of them, giving the parent the first.
pid_t forkpty(int *primary, char *name, const struct termios *settings,
              const struct winsize *size);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_PTY_FORKPTY_H

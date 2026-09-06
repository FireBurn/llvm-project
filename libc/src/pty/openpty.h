//===-- Implementation header for openpty -----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_PTY_OPENPTY_H
#define LLVM_LIBC_SRC_PTY_OPENPTY_H

#include "hdr/types/struct_termios.h"
#include "hdr/types/struct_winsize.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Opens a pair of terminals joined to each other, the one end standing in for
// the wire and the other for the terminal itself. `name`, where it is given,
// is filled in with the path of the second, and must have room for it.
int openpty(int *primary, int *secondary, char *name,
            const struct termios *settings, const struct winsize *size);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_PTY_OPENPTY_H

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of forkpty
///
//===----------------------------------------------------------------------===//

#include "src/pty/forkpty.h"

#include "src/__support/OSUtil/exit.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/pty/login_tty.h"
#include "src/pty/openpty.h"
#include "src/unistd/fork.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(pid_t, forkpty,
                   (int *primary, char *name, const struct termios *settings,
                    const struct winsize *size)) {
  int first = -1, second = -1;
  if (auto ret = internal::openpty_impl(&first, &second, name, settings, size);
      !ret) {
    libc_errno = ret.error();
    return -1;
  }

  ErrorOr<pid_t> child = internal::fork_impl();
  if (!child) {
    linux_syscalls::close(first);
    linux_syscalls::close(second);
    libc_errno = child.error();
    return -1;
  }

  if (*child == 0) {
    // The child keeps only the terminal end, and speaks through it.
    linux_syscalls::close(first);
    if (!internal::login_tty_impl(second))
      internal::exit(127);
    return 0;
  }

  linux_syscalls::close(second);
  if (primary != nullptr)
    *primary = first;
  return *child;
}

} // namespace LIBC_NAMESPACE_DECL

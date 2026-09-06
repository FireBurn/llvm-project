//===-- Implementation of forkpty -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pty/forkpty.h"

#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/pty/login_tty.h"
#include "src/pty/openpty.h"
#include "src/unistd/_exit.h"
#include "src/unistd/close.h"
#include "src/unistd/fork.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(pid_t, forkpty,
                   (int *primary, char *name, const struct termios *settings,
                    const struct winsize *size)) {
  int first = -1, second = -1;
  if (LIBC_NAMESPACE::openpty(&first, &second, name, settings, size) != 0)
    return -1;

  pid_t child = LIBC_NAMESPACE::fork();
  if (child < 0) {
    const int saved = libc_errno;
    LIBC_NAMESPACE::close(first);
    LIBC_NAMESPACE::close(second);
    libc_errno = saved;
    return -1;
  }

  if (child == 0) {
    // The child keeps only the terminal end, and speaks through it.
    LIBC_NAMESPACE::close(first);
    if (LIBC_NAMESPACE::login_tty(second) != 0)
      LIBC_NAMESPACE::_exit(127);
    return 0;
  }

  LIBC_NAMESPACE::close(second);
  if (primary != nullptr)
    *primary = first;
  return child;
}

} // namespace LIBC_NAMESPACE_DECL

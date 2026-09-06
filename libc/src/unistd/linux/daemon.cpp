//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of daemon
///
//===----------------------------------------------------------------------===//

#include "src/unistd/daemon.h"

#include "hdr/fcntl_macros.h"
#include "hdr/unistd_macros.h"
#include "src/__support/OSUtil/exit.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/chdir.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/dup2.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/open.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/setsid.h"
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/unistd/fork.h"

namespace LIBC_NAMESPACE_DECL {

// Leaves the caller running in the background, in a session of its own, so
// that it is not the child of the shell which started it and has no
// controlling terminal.
LLVM_LIBC_FUNCTION(int, daemon, (int nochdir, int noclose)) {
  ErrorOr<pid_t> child = internal::fork_impl();
  if (!child) {
    libc_errno = child.error();
    return -1;
  }
  // The parent is what the shell is waiting for, so it goes away and leaves
  // the child running. exit rather than the library's exit: the handlers and
  // the streams belong to the program, which is carrying on in the child.
  if (*child != 0)
    internal::exit(0);

  if (auto sid = linux_syscalls::setsid(); !sid) {
    libc_errno = sid.error();
    return -1;
  }

  // Nothing should be left holding a directory which cannot then be unmounted.
  if (!nochdir)
    linux_syscalls::chdir("/");

  // The three standard descriptors would otherwise still be on the terminal
  // the caller was started from.
  if (!noclose) {
    if (ErrorOr<int> fd = linux_syscalls::open("/dev/null", O_RDWR, 0)) {
      linux_syscalls::dup2(*fd, STDIN_FILENO);
      linux_syscalls::dup2(*fd, STDOUT_FILENO);
      linux_syscalls::dup2(*fd, STDERR_FILENO);
      if (*fd > STDERR_FILENO)
        linux_syscalls::close(*fd);
    }
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

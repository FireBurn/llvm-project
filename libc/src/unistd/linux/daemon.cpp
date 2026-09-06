//===-- Linux implementation of daemon ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/daemon.h"

#include "hdr/fcntl_macros.h"
#include "hdr/unistd_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/fcntl/open.h"
#include "src/stdlib/_Exit.h"
#include "src/unistd/chdir.h"
#include "src/unistd/close.h"
#include "src/unistd/dup2.h"
#include "src/unistd/fork.h"
#include "src/unistd/setsid.h"

namespace LIBC_NAMESPACE_DECL {

// Leaves the caller running in the background, in a session of its own, so
// that it is not the child of the shell which started it and has no
// controlling terminal.
LLVM_LIBC_FUNCTION(int, daemon, (int nochdir, int noclose)) {
  switch (LIBC_NAMESPACE::fork()) {
  case -1:
    return -1;
  case 0:
    break;
  default:
    // The parent is what the shell is waiting for, so it goes away and leaves
    // the child running. _Exit rather than exit: the handlers and the streams
    // belong to the program, which is carrying on in the child.
    LIBC_NAMESPACE::_Exit(0);
  }

  if (LIBC_NAMESPACE::setsid() == -1)
    return -1;

  // Nothing should be left holding a directory which cannot then be unmounted.
  if (!nochdir)
    LIBC_NAMESPACE::chdir("/");

  // The three standard descriptors would otherwise still be on the terminal
  // the caller was started from.
  if (!noclose) {
    int fd = LIBC_NAMESPACE::open("/dev/null", O_RDWR);
    if (fd != -1) {
      LIBC_NAMESPACE::dup2(fd, STDIN_FILENO);
      LIBC_NAMESPACE::dup2(fd, STDOUT_FILENO);
      LIBC_NAMESPACE::dup2(fd, STDERR_FILENO);
      if (fd > STDERR_FILENO)
        LIBC_NAMESPACE::close(fd);
    }
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

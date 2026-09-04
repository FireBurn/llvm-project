//===-- Implementation of pclose ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/pclose.h"

#include "hdr/errno_macros.h"
#include "hdr/types/FILE.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdio/fclose.h"
#include "src/stdio/linux/popen_registry.h"
#include "src/sys/wait/waitpid.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pclose, (::FILE * stream)) {
  pid_t pid = internal::popen_forget(stream);
  if (pid < 0) {
    // Not a stream popen returned.
    libc_errno = EINVAL;
    return -1;
  }

  // Closing first lets the child see end of file and finish, which is what
  // makes waiting for it terminate.
  if (LIBC_NAMESPACE::fclose(stream) != 0)
    return -1;

  int status = 0;
  // A signal delivered while waiting must not be mistaken for the child
  // exiting, so an interrupted wait is retried.
  for (;;) {
    pid_t result = LIBC_NAMESPACE::waitpid(pid, &status, 0);
    if (result == pid)
      return status;
    if (result < 0 && libc_errno == EINTR) {
      libc_errno = 0;
      continue;
    }
    return -1;
  }
}

} // namespace LIBC_NAMESPACE_DECL

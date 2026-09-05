//===-- Linux implementation of grantpt -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/grantpt.h"

#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/linux/pty_utils.h"

namespace LIBC_NAMESPACE_DECL {

// The devpts filesystem gives a terminal the right owner and mode when it
// is made, so there is nothing left to put right. What this still does is
// check the descriptor really is a master, since a caller which gets no
// error expects that much.
LLVM_LIBC_FUNCTION(int, grantpt, (int fd)) {
  unsigned int number = 0;
  if (int err = pty::number(fd, &number); err != 0) {
    libc_errno = err;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

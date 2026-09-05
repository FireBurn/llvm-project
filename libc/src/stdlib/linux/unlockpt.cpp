//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of unlockpt
///
//===----------------------------------------------------------------------===//

#include "src/stdlib/unlockpt.h"

#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/linux/pty_utils.h"

namespace LIBC_NAMESPACE_DECL {

// Lets the terminal behind the master be opened.
LLVM_LIBC_FUNCTION(int, unlockpt, (int fd)) {
  if (int err = pty::unlock(fd); err != 0) {
    libc_errno = err;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

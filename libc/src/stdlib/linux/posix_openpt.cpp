//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of posix_openpt
///
//===----------------------------------------------------------------------===//

#include "src/stdlib/posix_openpt.h"

#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/linux/pty_utils.h"
namespace LIBC_NAMESPACE_DECL {

// Opens the multiplexer, which hands back a master with a terminal of its
// own behind it.
LLVM_LIBC_FUNCTION(int, posix_openpt, (int flags)) {
  ErrorOr<int> fd = pty::open_master(flags);
  if (!fd) {
    libc_errno = fd.error();
    return -1;
  }
  return *fd;
}

} // namespace LIBC_NAMESPACE_DECL

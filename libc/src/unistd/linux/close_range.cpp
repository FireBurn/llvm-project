//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of close_range
///
//===----------------------------------------------------------------------===//

#include "src/unistd/close_range.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/close_range.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

// Closes every descriptor from first to last, which is far cheaper than
// walking /proc to find them.

LLVM_LIBC_FUNCTION(int, close_range,
                   (unsigned int first, unsigned int last, int flags)) {
  ErrorOr<int> ret = linux_syscalls::close_range(first, last, flags);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

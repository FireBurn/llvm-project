//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of ulckpwdf
///
//===----------------------------------------------------------------------===//

#include "src/shadow/ulckpwdf.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/shadow/pwdlock.h"
namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, ulckpwdf, (void)) {
  if (shadow_lock::held_fd < 0)
    return -1;
  // Closing the descriptor is what drops the lock.
  ErrorOr<int> ret = linux_syscalls::close(shadow_lock::held_fd);
  shadow_lock::held_fd = -1;
  return ret ? 0 : -1;
}

} // namespace LIBC_NAMESPACE_DECL

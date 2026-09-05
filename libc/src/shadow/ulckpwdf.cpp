//===-- Implementation of ulckpwdf ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/shadow/ulckpwdf.h"

#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/shadow/pwdlock.h"
#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, ulckpwdf, (void)) {
  if (shadow_lock::held_fd < 0)
    return -1;
  // Closing the descriptor is what drops the lock.
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_close, shadow_lock::held_fd);
  shadow_lock::held_fd = -1;
  return ret < 0 ? -1 : 0;
}

} // namespace LIBC_NAMESPACE_DECL

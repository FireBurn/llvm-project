//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of setuid.
///
//===----------------------------------------------------------------------===//

#include "src/unistd/setuid.h"

#include "hdr/types/uid_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/setuid.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/linux/setxid.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, setuid, (uid_t uid)) {
  auto ret = internal::setxid(
      [](long uid, long, long) {
        return linux_syscalls::setuid(static_cast<uid_t>(uid));
      },
      uid);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

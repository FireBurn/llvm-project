//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of setreuid.
///
//===----------------------------------------------------------------------===//

#include "src/unistd/setreuid.h"

#include "hdr/types/uid_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/setreuid.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/linux/setxid.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, setreuid, (uid_t ruid, uid_t euid)) {
  auto ret = internal::setxid(
      [](long ruid, long euid, long) {
        return linux_syscalls::setreuid(static_cast<uid_t>(ruid),
                                        static_cast<uid_t>(euid));
      },
      ruid, euid);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

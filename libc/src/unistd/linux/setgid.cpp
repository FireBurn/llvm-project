//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of setgid.
///
//===----------------------------------------------------------------------===//

#include "src/unistd/setgid.h"

#include "hdr/types/gid_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/setgid.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/linux/setxid.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, setgid, (gid_t gid)) {
  auto ret = internal::setxid(
      [](long gid, long, long) {
        return linux_syscalls::setgid(static_cast<gid_t>(gid));
      },
      gid);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

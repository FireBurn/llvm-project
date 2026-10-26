//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of setresgid.
///
//===----------------------------------------------------------------------===//

#include "src/unistd/setresgid.h"

#include "hdr/types/gid_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/setresgid.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/linux/setxid.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, setresgid, (gid_t rgid, gid_t egid, gid_t sgid)) {
  auto ret = internal::setxid(
      [](long rgid, long egid, long sgid) {
        return linux_syscalls::setresgid(static_cast<gid_t>(rgid),
                                         static_cast<gid_t>(egid),
                                         static_cast<gid_t>(sgid));
      },
      rgid, egid, sgid);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

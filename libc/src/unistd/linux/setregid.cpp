//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of setregid.
///
//===----------------------------------------------------------------------===//

#include "src/unistd/setregid.h"

#include "hdr/types/gid_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/setregid.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/linux/setxid.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, setregid, (gid_t rgid, gid_t egid)) {
  auto ret = internal::setxid(
      [](long rgid, long egid, long) {
        return linux_syscalls::setregid(static_cast<gid_t>(rgid),
                                        static_cast<gid_t>(egid));
      },
      rgid, egid);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

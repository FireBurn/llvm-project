//===-- Linux implementation of inotify_init1 -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/inotify/inotify_init1.h"

#include "hdr/stdint_proxy.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// A descriptor which becomes readable when something happens to a file or
// a directory being watched.
LLVM_LIBC_FUNCTION(int, inotify_init1, (int flags)) {
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_inotify_init1, flags);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL

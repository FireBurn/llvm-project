//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of the inotify_rm_watch function.
///
//===----------------------------------------------------------------------===//

#include "src/sys/inotify/inotify_rm_watch.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/inotify_rm_watch.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, inotify_rm_watch, (int fd, int wd)) {
  ErrorOr<int> ret = linux_syscalls::inotify_rm_watch(fd, wd);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return ret.value();
}

} // namespace LIBC_NAMESPACE_DECL
//===-- Linux implementation of inotify_rm_watch --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/inotify/inotify_rm_watch.h"

#include "hdr/stdint_proxy.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, inotify_rm_watch, (int fd, int wd)) {
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_inotify_rm_watch, fd, wd);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of fanotify_init
///
//===----------------------------------------------------------------------===//

#include "src/sys/fanotify/fanotify_init.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/fanotify_init.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, fanotify_init,
                   (unsigned int flags, unsigned int event_f_flags)) {
  ErrorOr<int> ret = linux_syscalls::fanotify_init(flags, event_f_flags);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return *ret;
}

} // namespace LIBC_NAMESPACE_DECL

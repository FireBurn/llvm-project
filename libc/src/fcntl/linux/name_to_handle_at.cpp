//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of name_to_handle_at
///
//===----------------------------------------------------------------------===//

#include "src/fcntl/name_to_handle_at.h"

#include "hdr/types/struct_file_handle.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/name_to_handle_at.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, name_to_handle_at,
                   (int dirfd, const char *pathname, struct file_handle *handle,
                    int *mount_id, int flags)) {
  ErrorOr<int> ret = linux_syscalls::name_to_handle_at(dirfd, pathname, handle,
                                                       mount_id, flags);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

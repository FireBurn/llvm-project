//===-- Linux implementation of mount_setattr ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/mount/mount_setattr.h"

#include "hdr/types/size_t.h"
#include "hdr/types/struct_mount_attr.h"
#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, mount_setattr,
                   (int dfd, const char *path, unsigned int flags,
                    struct mount_attr *attr, size_t size)) {
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_mount_setattr, dfd, path,
                                              flags, attr, size);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

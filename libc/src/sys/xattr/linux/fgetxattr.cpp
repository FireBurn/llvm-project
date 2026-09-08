//===-- Linux implementation of fgetxattr ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/xattr/fgetxattr.h"

#include "hdr/types/size_t.h"
#include "hdr/types/ssize_t.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(ssize_t, fgetxattr,
                   (int fd, const char *name, void *value, size_t size)) {
  long ret =
      LIBC_NAMESPACE::syscall_impl<long>(SYS_fgetxattr, fd, name, value, size);
  if (ret < 0) {
    libc_errno = static_cast<int>(-ret);
    return -1;
  }
  return static_cast<ssize_t>(ret);
}

} // namespace LIBC_NAMESPACE_DECL

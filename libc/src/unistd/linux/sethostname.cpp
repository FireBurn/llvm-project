//===-- Linux implementation of sethostname ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/sethostname.h"

#include "hdr/types/size_t.h"
#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, sethostname, (const char *name, size_t len)) {
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_sethostname, name, len);
  if (ret < 0) {
    libc_errno = (-ret);
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

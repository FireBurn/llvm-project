//===-- Linux implementation of getdomainname -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/getdomainname.h"

#include "hdr/errno_macros.h"
#include "hdr/types/struct_utsname.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/string/string_utils.h"

#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

// The kernel keeps the domain name beside the rest of what uname reports,
// so it is read from there rather than through a call of its own.
LLVM_LIBC_FUNCTION(int, getdomainname, (char *name, size_t len)) {
  if (name == nullptr) {
    libc_errno = EINVAL;
    return -1;
  }

  struct utsname info;
  int ret = syscall_impl<int>(SYS_uname, &info);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }

  const size_t written = internal::string_length(info.domainname);
  if (written + 1 > len) {
    libc_errno = EINVAL;
    return -1;
  }
  inline_memcpy(name, info.domainname, written + 1);
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

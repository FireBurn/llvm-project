//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of setegid.
///
//===----------------------------------------------------------------------===//

#include "src/unistd/setegid.h"

#include "hdr/types/gid_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/setregid.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Setting only the effective id is setregid with the real one left alone,
// which is what -1 means to the kernel.
LLVM_LIBC_FUNCTION(int, setegid, (gid_t id)) {
  auto ret = linux_syscalls::setregid(static_cast<gid_t>(-1), id);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

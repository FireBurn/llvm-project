//===-- Linux implementation of setfsgid ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/fsuid/setfsgid.h"

#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

// Which group the kernel takes this thread to be when it checks a file. It is
// separate from the real and effective user so that a server can act for
// somebody else over a file without giving up anything else.
//
// The call always reports the group it was before, and never fails: a caller
// which wants to know whether the change took has to ask again.
LLVM_LIBC_FUNCTION(int, setfsgid, (gid_t id)) {
  return syscall_impl<int>(SYS_setfsgid, id);
}

} // namespace LIBC_NAMESPACE_DECL

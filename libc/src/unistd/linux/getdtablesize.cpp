//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of getdtablesize
///
//===----------------------------------------------------------------------===//

#include "src/unistd/getdtablesize.h"

#include "hdr/limits_macros.h"
#include "hdr/sys_resource_macros.h"
#include "hdr/types/struct_rlimit.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/prlimit64.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// How many descriptors the process may have, which is the soft limit on
// open files. There is no way to report a failure, so a limit which cannot
// be read is answered with the compiled in one.
LLVM_LIBC_FUNCTION(int, getdtablesize, (void)) {
  struct rlimit limit;
  if (linux_syscalls::prlimit64(0, RLIMIT_NOFILE, nullptr, &limit) &&
      limit.rlim_cur != RLIM_INFINITY)
    return static_cast<int>(limit.rlim_cur);
  // POSIX's floor on how many a process may have, which is the least this
  // can honestly claim.
  return _POSIX_OPEN_MAX;
}

} // namespace LIBC_NAMESPACE_DECL

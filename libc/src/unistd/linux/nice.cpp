//===-- Linux implementation of nice --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/nice.h"

#include "hdr/errno_macros.h"
#include "hdr/sys_resource_macros.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// The kernel has no nice call of its own on any architecture this builds for,
// so this is the read and write of the priority that the C interface names.
// The value returned is the new one, which a caller cannot tell from a
// failure by itself: errno is cleared first, so that it says which it was.
LLVM_LIBC_FUNCTION(int, nice, (int increment)) {
  // getpriority answers with the value biased away from negative numbers, so
  // that the kernel can report a failure as a negative return.
  const long biased =
      LIBC_NAMESPACE::syscall_impl<long>(SYS_getpriority, PRIO_PROCESS, 0);
  if (biased < 0) {
    libc_errno = static_cast<int>(-biased);
    return -1;
  }
  const int current = static_cast<int>(PRIO_MAX - biased);

  long wanted = static_cast<long>(current) + increment;
  // A request past either end is met at the end rather than refused, which is
  // what every other implementation of this does.
  if (wanted < PRIO_MIN)
    wanted = PRIO_MIN;
  if (wanted > PRIO_MAX - 1)
    wanted = PRIO_MAX - 1;

  const long result = LIBC_NAMESPACE::syscall_impl<long>(
      SYS_setpriority, PRIO_PROCESS, 0, wanted);
  if (result < 0) {
    libc_errno = static_cast<int>(-result);
    return -1;
  }
  return static_cast<int>(wanted);
}

} // namespace LIBC_NAMESPACE_DECL

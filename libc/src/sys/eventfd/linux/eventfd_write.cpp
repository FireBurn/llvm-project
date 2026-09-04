//===-- Linux implementation of eventfd_write -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/eventfd/eventfd_write.h"

#include "hdr/types/eventfd_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/write.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// A write of anything but the whole counter is an error, so this reports
// whether the one write of that size happened rather than how much it added.
LLVM_LIBC_FUNCTION(int, eventfd_write, (int fd, eventfd_t value)) {
  auto result = linux_syscalls::write(fd, &value, sizeof(eventfd_t));
  if (!result.has_value()) {
    libc_errno = result.error();
    return -1;
  }
  return result.value() == sizeof(eventfd_t) ? 0 : -1;
}

} // namespace LIBC_NAMESPACE_DECL

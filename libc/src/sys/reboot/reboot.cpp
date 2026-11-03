//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of reboot
///
//===----------------------------------------------------------------------===//

#include "src/sys/reboot/reboot.h"

#include "hdr/sys_reboot_macros.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/reboot.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, reboot, (int howto)) {
  // The magic numbers are the kernel's guard against a stray call, and the
  // wrapper supplies them so a caller does not have to.
  ErrorOr<int> ret = linux_syscalls::reboot(
      static_cast<int>(LINUX_REBOOT_MAGIC1),
      static_cast<int>(LINUX_REBOOT_MAGIC2), howto, nullptr);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return *ret;
}

} // namespace LIBC_NAMESPACE_DECL

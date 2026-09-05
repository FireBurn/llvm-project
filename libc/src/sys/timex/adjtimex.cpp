//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of adjtimex
///
//===----------------------------------------------------------------------===//

#include "src/sys/timex/adjtimex.h"

#include "hdr/types/struct_timex.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/adjtimex.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, adjtimex, (struct timex * buf)) {
  // What comes back is the clock's state with respect to a leap second,
  // which is not an error, so it is returned as it stands.
  ErrorOr<int> ret = linux_syscalls::adjtimex(buf);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return *ret;
}

} // namespace LIBC_NAMESPACE_DECL

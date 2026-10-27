//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of clock_nanosleep function.
///
//===----------------------------------------------------------------------===//

#include "src/time/clock_nanosleep.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/clock_nanosleep.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, clock_nanosleep,
                   (clockid_t clockid, int flags, const struct timespec *req,
                    struct timespec *rem)) {
  ErrorOr<int> ret = linux_syscalls::clock_nanosleep(clockid, flags, req, rem);
  // Unlike almost everything else, this returns the error rather than
  // setting errno, so a caller can tell an interrupted sleep apart without
  // clearing errno first.
  if (!ret)
    return ret.error();
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

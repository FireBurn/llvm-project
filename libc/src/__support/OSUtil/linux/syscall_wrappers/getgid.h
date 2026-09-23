//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Syscall wrapper for getgid.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_GETGID_H
#define LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_GETGID_H

#include "hdr/types/gid_t.h"
#include "src/__support/OSUtil/linux/syscall.h" // syscall_impl
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers

namespace LIBC_NAMESPACE_DECL {
namespace linux_syscalls {

LIBC_INLINE gid_t getgid() {
  // getgid always succeeds, so there is no error checking here.
#if defined(SYS_getgid32)
  return syscall_impl<gid_t>(SYS_getgid32);
#elif defined(SYS_getgid)
  return syscall_impl<gid_t>(SYS_getgid);
#else
#error "SYS_getgid and SYS_getgid32 syscalls not available."
#endif
}

} // namespace linux_syscalls
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_GETGID_H

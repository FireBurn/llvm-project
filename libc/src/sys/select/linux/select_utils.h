//===-- Linux helpers for the select family ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_SELECT_LINUX_SELECT_UTILS_H
#define LLVM_LIBC_SRC_SYS_SELECT_LINUX_SELECT_UTILS_H

#include "hdr/types/sigset_t.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {
namespace internal {

// The last argument of SYS_pselect6 is a pointer to this pair rather than to
// the mask itself.
struct pselect6_sigset_t {
  const sigset_t *ss;
  size_t ss_len;
};

// A null |ts| means block until a descriptor is ready. The kernel updates the
// timespec it is given, so callers which must not expose that pass a copy.
LIBC_INLINE int pselect6(int nfds, fd_set *__restrict read_set,
                         fd_set *__restrict write_set,
                         fd_set *__restrict error_set, struct timespec *ts,
                         pselect6_sigset_t *pss) {
#if defined(SYS_pselect6_time64)
  return LIBC_NAMESPACE::syscall_impl<int>(SYS_pselect6_time64, nfds, read_set,
                                           write_set, error_set, ts, pss);
#elif defined(SYS_pselect6)
  static_assert(
      sizeof(timespec::tv_nsec) == sizeof(long),
      "This legacy syscall fallback is only safe on platforms where tv_nsec "
      "matches the register size (long). It is unsafe on 32-bit platforms "
      "with 64-bit tv_nsec.");
  return LIBC_NAMESPACE::syscall_impl<int>(SYS_pselect6, nfds, read_set,
                                           write_set, error_set, ts, pss);
#else
#error "SYS_pselect6 and SYS_pselect6_time64 syscalls not available."
#endif
}

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_SELECT_LINUX_SELECT_UTILS_H

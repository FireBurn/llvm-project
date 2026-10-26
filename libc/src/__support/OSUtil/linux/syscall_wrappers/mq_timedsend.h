//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Syscall wrapper for mq_timedsend.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_MQ_TIMEDSEND_H
#define LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_MQ_TIMEDSEND_H

#include "hdr/types/mqd_t.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/OSUtil/linux/syscall.h" // syscall_checked
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers

namespace LIBC_NAMESPACE_DECL {
namespace linux_syscalls {

LIBC_INLINE ErrorOr<int> mq_timedsend(mqd_t mqdes, const char *msg_ptr,
                                      size_t msg_len, unsigned int msg_prio,
                                      const struct timespec *abs_timeout) {
  return syscall_checked<int>(SYS_mq_timedsend, mqdes, msg_ptr, msg_len,
                              msg_prio, abs_timeout);
}

} // namespace linux_syscalls
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_MQ_TIMEDSEND_H

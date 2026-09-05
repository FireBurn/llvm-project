//===-- Implementation of mq_notify ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/mqueue/mq_notify.h"

#include "hdr/types/mqd_t.h"
#include "hdr/types/struct_sigevent.h"
#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

// Asking to be told when a message arrives at an empty queue. Only a signal
// or nothing at all is passed to the kernel; SIGEV_THREAD, which asks for a
// thread to be started, is not supported here.
LLVM_LIBC_FUNCTION(int, mq_notify, (mqd_t mqdes, const struct sigevent *sevp)) {
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_mq_notify, mqdes, sevp);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

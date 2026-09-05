//===-- Implementation of mq_timedreceive ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/mqueue/mq_timedreceive.h"

#include "hdr/types/mqd_t.h"
#include "hdr/types/size_t.h"
#include "hdr/types/ssize_t.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(ssize_t, mq_timedreceive,
                   (mqd_t mqdes, char *msg_ptr, size_t msg_len,
                    unsigned int *msg_prio,
                    const struct timespec *abs_timeout)) {
  ssize_t ret = LIBC_NAMESPACE::syscall_impl<ssize_t>(
      SYS_mq_timedreceive, mqdes, msg_ptr, msg_len, msg_prio, abs_timeout);
  if (ret < 0) {
    libc_errno = static_cast<int>(-ret);
    return -1;
  }
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of mq_send
///
//===----------------------------------------------------------------------===//

#include "src/mqueue/mq_send.h"

#include "hdr/types/mqd_t.h"
#include "hdr/types/size_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/mq_timedsend.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Waiting without a deadline is the timed call with no deadline given.
LLVM_LIBC_FUNCTION(int, mq_send,
                   (mqd_t mqdes, const char *msg_ptr, size_t msg_len,
                    unsigned int msg_prio)) {
  ErrorOr<int> ret =
      linux_syscalls::mq_timedsend(mqdes, msg_ptr, msg_len, msg_prio, nullptr);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

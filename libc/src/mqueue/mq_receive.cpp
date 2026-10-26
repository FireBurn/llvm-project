//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of mq_receive
///
//===----------------------------------------------------------------------===//

#include "src/mqueue/mq_receive.h"

#include "hdr/types/mqd_t.h"
#include "hdr/types/size_t.h"
#include "hdr/types/ssize_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/mq_timedreceive.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(ssize_t, mq_receive,
                   (mqd_t mqdes, char *msg_ptr, size_t msg_len,
                    unsigned int *msg_prio)) {
  ErrorOr<ssize_t> ret = linux_syscalls::mq_timedreceive(
      mqdes, msg_ptr, msg_len, msg_prio, nullptr);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return *ret;
}

} // namespace LIBC_NAMESPACE_DECL

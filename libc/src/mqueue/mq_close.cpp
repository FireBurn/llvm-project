//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of mq_close
///
//===----------------------------------------------------------------------===//

#include "src/mqueue/mq_close.h"

#include "hdr/types/mqd_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

// A queue is a descriptor, so letting go of it is just a close.
LLVM_LIBC_FUNCTION(int, mq_close, (mqd_t mqdes)) {
  ErrorOr<int> ret = linux_syscalls::close(mqdes);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

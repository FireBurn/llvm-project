//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of mq_getattr
///
//===----------------------------------------------------------------------===//

#include "src/mqueue/mq_getattr.h"

#include "hdr/types/mqd_t.h"
#include "hdr/types/struct_mq_attr.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/mq_getsetattr.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

// The kernel has one call for both directions; reading is that call with
// nothing to set.
LLVM_LIBC_FUNCTION(int, mq_getattr, (mqd_t mqdes, struct mq_attr *attr)) {
  ErrorOr<int> ret = linux_syscalls::mq_getsetattr(mqdes, nullptr, attr);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of mq_setattr
///
//===----------------------------------------------------------------------===//

#include "src/mqueue/mq_setattr.h"

#include "hdr/types/mqd_t.h"
#include "hdr/types/struct_mq_attr.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/mq_getsetattr.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

// Only mq_flags is looked at; the rest of the structure was fixed when the
// queue was made.
LLVM_LIBC_FUNCTION(int, mq_setattr,
                   (mqd_t mqdes, const struct mq_attr *__restrict newattr,
                    struct mq_attr *__restrict oldattr)) {
  ErrorOr<int> ret = linux_syscalls::mq_getsetattr(mqdes, newattr, oldattr);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

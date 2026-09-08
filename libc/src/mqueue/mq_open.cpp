//===-- Implementation of mq_open -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/mqueue/mq_open.h"

#include "hdr/fcntl_macros.h"
#include "hdr/types/mode_t.h"
#include "hdr/types/mqd_t.h"
#include "hdr/types/struct_mq_attr.h"
#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <stdarg.h>
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(mqd_t, mq_open, (const char *name, int oflag, ...)) {
  mode_t mode = 0;
  struct mq_attr *attr = nullptr;

  // The mode and the attributes are only there when the queue is being
  // created.
  if (oflag & O_CREAT) {
    va_list args;
    va_start(args, oflag);
    mode = static_cast<mode_t>(va_arg(args, unsigned int));
    attr = va_arg(args, struct mq_attr *);
    va_end(args);
  }

  // A queue name begins with a slash which is not part of the name the
  // kernel wants.
  const char *bare = name;
  if (bare != nullptr && bare[0] == '/')
    ++bare;

  int ret =
      LIBC_NAMESPACE::syscall_impl<int>(SYS_mq_open, bare, oflag, mode, attr);
  if (ret < 0) {
    libc_errno = -ret;
    return static_cast<mqd_t>(-1);
  }
  return static_cast<mqd_t>(ret);
}

} // namespace LIBC_NAMESPACE_DECL

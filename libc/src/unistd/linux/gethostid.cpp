//===-- Linux implementation of gethostid ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/gethostid.h"

#include "hdr/fcntl_macros.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// Where the number is kept, for as long as anything has cared to set one.
// A machine without the file has no identifier to give, and zero is the
// answer musl gives in that case. POSIX marks this obsolescent and asks
// nothing of the value beyond its being an identifier.
LLVM_LIBC_FUNCTION(long, gethostid, (void)) {
  int fd = LIBC_NAMESPACE::syscall_impl<int>(SYS_openat, AT_FDCWD,
                                             "/etc/hostid", O_RDONLY, 0);
  if (fd < 0)
    return 0;

  int32_t id = 0;
  long read_result = LIBC_NAMESPACE::syscall_impl<long>(
      SYS_read, fd, reinterpret_cast<long>(&id), sizeof(id));
  LIBC_NAMESPACE::syscall_impl<long>(SYS_close, fd);

  if (read_result != static_cast<long>(sizeof(id)))
    return 0;
  return id;
}

} // namespace LIBC_NAMESPACE_DECL

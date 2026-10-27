//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of gethostid
///
//===----------------------------------------------------------------------===//

#include "src/unistd/gethostid.h"

#include "hdr/fcntl_macros.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/openat.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/read.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Where the number is kept, for as long as anything has cared to set one.
// A machine without the file has no identifier to give, and zero is the
// answer musl gives in that case. POSIX marks this obsolescent and asks
// nothing of the value beyond its being an identifier.
LLVM_LIBC_FUNCTION(long, gethostid, (void)) {
  ErrorOr<int> fd =
      linux_syscalls::openat(AT_FDCWD, "/etc/hostid", O_RDONLY, 0);
  if (!fd)
    return 0;

  int32_t id = 0;
  ErrorOr<ssize_t> read_result = linux_syscalls::read(*fd, &id, sizeof(id));
  linux_syscalls::close(*fd);

  if (!read_result || *read_result != static_cast<ssize_t>(sizeof(id)))
    return 0;
  return id;
}

} // namespace LIBC_NAMESPACE_DECL

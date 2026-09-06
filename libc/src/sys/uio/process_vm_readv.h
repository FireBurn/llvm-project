//===-- Implementation header for process_vm_readv ------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_UIO_PROCESS_VM_READV_H
#define LLVM_LIBC_SRC_SYS_UIO_PROCESS_VM_READV_H

#include "hdr/types/pid_t.h"
#include "hdr/types/ssize_t.h"
#include "hdr/types/struct_iovec.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Moves data between this process's memory and another's without going
// through a file. Reaching into another process this way needs the right to
// trace it.
ssize_t process_vm_readv(pid_t pid, const struct iovec *local_iov,
                         unsigned long liovcnt, const struct iovec *remote_iov,
                         unsigned long riovcnt, unsigned long flags);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_UIO_PROCESS_VM_READV_H

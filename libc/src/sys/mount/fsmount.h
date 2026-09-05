//===-- Implementation header for fsmount -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_MOUNT_FSMOUNT_H
#define LLVM_LIBC_SRC_SYS_MOUNT_FSMOUNT_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int fsmount(int fd, unsigned int flags, unsigned int attr_flags);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_MOUNT_FSMOUNT_H

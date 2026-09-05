//===-- Implementation header for move_mount --------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_MOUNT_MOVE_MOUNT_H
#define LLVM_LIBC_SRC_SYS_MOUNT_MOVE_MOUNT_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int move_mount(int from_dfd, const char *from_path, int to_dfd,
               const char *to_path, unsigned int flags);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_MOUNT_MOVE_MOUNT_H

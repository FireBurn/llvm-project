//===-- Implementation header for fsconfig ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_MOUNT_FSCONFIG_H
#define LLVM_LIBC_SRC_SYS_MOUNT_FSCONFIG_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int fsconfig(int fd, unsigned int cmd, const char *key, const void *value,
             int aux);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_MOUNT_FSCONFIG_H

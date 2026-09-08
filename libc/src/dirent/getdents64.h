//===-- Implementation header for getdents64 --------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_DIRENT_GETDENTS64_H
#define LLVM_LIBC_SRC_DIRENT_GETDENTS64_H

#include "hdr/types/size_t.h"
#include "hdr/types/ssize_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

ssize_t getdents64(int fd, void *buf, size_t count);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_DIRENT_GETDENTS64_H

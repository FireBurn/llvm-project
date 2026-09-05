//===-- Implementation header for open_by_handle_at -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_FCNTL_OPEN_BY_HANDLE_AT_H
#define LLVM_LIBC_SRC_FCNTL_OPEN_BY_HANDLE_AT_H

#include "hdr/types/struct_file_handle.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int open_by_handle_at(int mount_fd, struct file_handle *handle, int flags);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_FCNTL_OPEN_BY_HANDLE_AT_H

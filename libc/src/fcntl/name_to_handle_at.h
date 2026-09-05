//===-- Implementation header for name_to_handle_at -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_FCNTL_NAME_TO_HANDLE_AT_H
#define LLVM_LIBC_SRC_FCNTL_NAME_TO_HANDLE_AT_H

#include "hdr/types/struct_file_handle.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int name_to_handle_at(int dirfd, const char *pathname,
                      struct file_handle *handle, int *mount_id, int flags);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_FCNTL_NAME_TO_HANDLE_AT_H

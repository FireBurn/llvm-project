//===-- Implementation header for get_current_dir_name ----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_UNISTD_GET_CURRENT_DIR_NAME_H
#define LLVM_LIBC_SRC_UNISTD_GET_CURRENT_DIR_NAME_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

char *get_current_dir_name(void);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_UNISTD_GET_CURRENT_DIR_NAME_H

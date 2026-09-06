//===-- Implementation header for gnu_get_libc_version ----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_COMPAT_GNU_GET_LIBC_VERSION_H
#define LLVM_LIBC_SRC_COMPAT_GNU_GET_LIBC_VERSION_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

const char *gnu_get_libc_version(void);
const char *gnu_get_libc_release(void);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_COMPAT_GNU_GET_LIBC_VERSION_H

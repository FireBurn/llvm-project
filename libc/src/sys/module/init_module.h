//===-- Implementation header for init_module -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_MODULE_INIT_MODULE_H
#define LLVM_LIBC_SRC_SYS_MODULE_INIT_MODULE_H

#include "hdr/types/size_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

long init_module(const void *module_image, unsigned long len,
                 const char *param_values);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_MODULE_INIT_MODULE_H

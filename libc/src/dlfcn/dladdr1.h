//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for dladdr1
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_DLFCN_DLADDR1_H
#define LLVM_LIBC_SRC_DLFCN_DLADDR1_H

#include "src/__support/macros/config.h"

#include "hdr/types/dl_info.h"

namespace LIBC_NAMESPACE_DECL {

int dladdr1(const void *__restrict, Dl_info *__restrict, void **__restrict,
            int);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_DLFCN_DLADDR1_H

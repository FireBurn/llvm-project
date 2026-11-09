//===-- Implementation header of dlopen -------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_DLFCN_DLOPEN_H
#define LLVM_LIBC_SRC_DLFCN_DLOPEN_H

#include "src/__support/elf/passive_abi.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_namespace.h"

namespace LIBC_NAMESPACE_DECL {

void *dlopen(const char *, int);

namespace internal {
void *dlopen_impl(const char *path, int mode);

// The same for the code at `caller`, opening in `ns`, or in the namespace of
// whatever `caller` is in when that is null.
void *dlopen_in(const char *path, int mode, ElfW(Addr) caller,
                dl::Namespace *ns);
} // namespace internal

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_DLFCN_DLOPEN_H

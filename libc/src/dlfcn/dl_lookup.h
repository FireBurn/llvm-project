//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Symbol lookup shared by dlsym and dlvsym
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_DLFCN_DL_LOOKUP_H
#define LLVM_LIBC_SRC_DLFCN_DL_LOOKUP_H

#include "src/__support/elf/passive_abi.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace dl {

// Finds `name` the way dlsym does. `caller` is an address in whatever called
// dlsym, which RTLD_NEXT needs, and so has to be taken by the entry point.
void *lookup(void *handle, const char *name, ElfW(Addr) caller,
             const char *version = nullptr);

} // namespace dl
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_DLFCN_DL_LOOKUP_H

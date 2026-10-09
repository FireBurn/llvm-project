//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Finding the object and symbol an address belongs to
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_DLFCN_DL_ADDRESS_H
#define LLVM_LIBC_SRC_DLFCN_DL_ADDRESS_H

#include "hdr/types/dl_info.h"
#include "hdr/types/struct_link_map.h"
#include "src/__support/elf/passive_abi.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace dl {

// Says which object `addr` is in, and which symbol of it, filling `info`. The
// symbol and the load address of the object are given too when asked for.
// Returns false when it is in none, leaving dladdr's convention to the caller.
bool address_lookup(const void *addr, Dl_info *info, const ElfW(Sym) * *symbol,
                    ElfW(Addr) * bias);

// The entry of the chain a debugger sees which stands for the object loaded
// at `bias`. The chain is built from the module set, so the load address
// tells them apart.
struct link_map *link_map_for(ElfW(Addr) bias);

} // namespace dl
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_DLFCN_DL_ADDRESS_H

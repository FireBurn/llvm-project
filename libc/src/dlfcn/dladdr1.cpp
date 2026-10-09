//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of dladdr1
///
//===----------------------------------------------------------------------===//

#include "dladdr1.h"

#include "src/__support/common.h"
#include "src/__support/elf/passive_abi.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_address.h"

namespace LIBC_NAMESPACE_DECL {

namespace {
// What the last argument asks to have stored, with the values <dlfcn.h> gives.
constexpr int DL_SYMENT = 1;
constexpr int DL_LINKMAP = 2;
} // anonymous namespace

// dladdr, and with it the symbol table entry of the symbol found or the link
// map entry of the object, as the flags say.
LLVM_LIBC_FUNCTION(int, dladdr1,
                   (const void *__restrict addr, Dl_info *__restrict info,
                    void **__restrict extra_info, int flags)) {
  const ElfW(Sym) *symbol = nullptr;
  ElfW(Addr) bias = 0;
  if (!dl::address_lookup(addr, info, &symbol, &bias))
    return 0;
  if (extra_info != nullptr) {
    if (flags == DL_SYMENT)
      *extra_info = const_cast<ElfW(Sym) *>(symbol);
    else if (flags == DL_LINKMAP)
      *extra_info = dl::link_map_for(bias);
  }
  return 1;
}

} // namespace LIBC_NAMESPACE_DECL

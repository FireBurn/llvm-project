//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of dlvsym
///
//===----------------------------------------------------------------------===//

#include "dlvsym.h"

#include "src/__support/common.h"
#include "src/__support/elf/passive_abi.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_lookup.h"

namespace LIBC_NAMESPACE_DECL {

// Nothing here defines more than one version of a symbol, so the version
// names the one there is and is not compared.
LLVM_LIBC_FUNCTION(void *, dlvsym,
                   (void *__restrict handle, const char *__restrict name,
                    const char *__restrict)) {
  const ElfW(Addr) caller =
      reinterpret_cast<ElfW(Addr)>(__builtin_return_address(0));
  return dl::lookup(handle, name, caller);
}

} // namespace LIBC_NAMESPACE_DECL

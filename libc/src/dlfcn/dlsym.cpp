//===-- Implementation of dlsym -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "dlsym.h"

#include "src/__support/common.h"
#include "src/__support/elf/passive_abi.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_lookup.h"

namespace LIBC_NAMESPACE_DECL {

namespace internal {
void *dlsym_impl(void *__restrict handle, const char *__restrict name) {
  // Which module the call is returning to, for RTLD_NEXT. Taken before
  // anything else, so nothing stands between this and the caller.
  const ElfW(Addr) caller =
      reinterpret_cast<ElfW(Addr)>(__builtin_return_address(0));
  return dl::lookup(handle, name, caller);
}

} // namespace internal

LLVM_LIBC_FUNCTION(void *, dlsym,
                   (void *__restrict handle, const char *__restrict name)) {
  return internal::dlsym_impl(handle, name);
}

} // namespace LIBC_NAMESPACE_DECL

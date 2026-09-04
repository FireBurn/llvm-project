//===-- Implementation of globfree ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/glob/globfree.h"

#include "hdr/func/free.h"
#include "hdr/types/glob_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, globfree, (glob_t * pglob)) {
  if (pglob == nullptr || pglob->gl_pathv == nullptr)
    return;
  // The slots GLOB_DOOFFS reserved at the front were never filled in here,
  // so only the matches themselves are freed.
  for (size_t i = 0; i < pglob->gl_pathc; ++i)
    ::free(pglob->gl_pathv[pglob->gl_offs + i]);
  ::free(pglob->gl_pathv);
  pglob->gl_pathv = nullptr;
  pglob->gl_pathc = 0;
}

} // namespace LIBC_NAMESPACE_DECL

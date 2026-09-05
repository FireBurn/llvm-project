//===-- Implementation of nftw -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/ftw/nftw.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/ftw/ftw_walk.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, nftw,
                   (const char *path, __nftw_func_t callback, int descriptors,
                    int flags)) {
  return ftw_internal::walk_tree(
      path, ftw_internal::Callback{nullptr, callback}, descriptors, flags);
}

} // namespace LIBC_NAMESPACE_DECL

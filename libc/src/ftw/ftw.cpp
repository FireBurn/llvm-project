//===-- Implementation of ftw -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/ftw/ftw.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/ftw/ftw_walk.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, ftw,
                   (const char *path, __ftw_func_t callback, int descriptors)) {
  return ftw_internal::walk_tree(
      path, ftw_internal::Callback{callback, nullptr}, descriptors, 0);
}

} // namespace LIBC_NAMESPACE_DECL

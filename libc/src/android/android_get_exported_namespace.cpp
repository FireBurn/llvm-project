//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_get_exported_namespace.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_get_exported_namespace.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// No namespace is exported, which callers see as the name not existing.
LLVM_LIBC_FUNCTION(struct android_namespace_t *, android_get_exported_namespace,
                   (const char *)) {
  return nullptr;
}

} // namespace LIBC_NAMESPACE_DECL

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
#include "src/__support/CPP/mutex.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_internal.h"
#include "src/dlfcn/dl_namespace.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(struct android_namespace_t *, android_get_exported_namespace,
                   (const char *name)) {
  if (name == nullptr)
    return nullptr;
  cpp::lock_guard lock(dl::dl_mutex);
  return reinterpret_cast<struct android_namespace_t *>(
      dl::find_exported_namespace(name));
}

} // namespace LIBC_NAMESPACE_DECL

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_init_anonymous_namespace.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_init_anonymous_namespace.h"
#include "src/__support/CPP/mutex.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_internal.h"
#include "src/dlfcn/dl_namespace.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(bool, android_init_anonymous_namespace,
                   (const char *shared_libs_sonames,
                    const char *library_search_path)) {
  cpp::lock_guard lock(dl::dl_mutex);
  return dl::init_anonymous_namespace(shared_libs_sonames, library_search_path);
}

} // namespace LIBC_NAMESPACE_DECL

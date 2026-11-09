//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_link_namespaces.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_link_namespaces.h"
#include "src/__support/CPP/mutex.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_internal.h"
#include "src/dlfcn/dl_namespace.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(bool, android_link_namespaces,
                   (struct android_namespace_t * namespace_from,
                    struct android_namespace_t *namespace_to,
                    const char *shared_libs_sonames)) {
  cpp::lock_guard lock(dl::dl_mutex);
  // The sonames of nothing make no link at all, as a loader's config does.
  if (shared_libs_sonames == nullptr) {
    dl::set_error("android_link_namespaces needs the sonames to share");
    return false;
  }
  return dl::link_namespaces(reinterpret_cast<dl::Namespace *>(namespace_from),
                             reinterpret_cast<dl::Namespace *>(namespace_to),
                             shared_libs_sonames);
}

} // namespace LIBC_NAMESPACE_DECL

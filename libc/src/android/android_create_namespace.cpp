//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_create_namespace.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_create_namespace.h"
#include "src/__support/CPP/mutex.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_internal.h"
#include "src/dlfcn/dl_namespace.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(struct android_namespace_t *, android_create_namespace,
                   (const char *name, const char *ld_library_path,
                    const char *default_library_path, uint64_t type,
                    const char *permitted_when_isolated_path,
                    struct android_namespace_t *parent)) {
  // Without a parent, the namespace of whatever called is the one.
  const ElfW(Addr) caller =
      reinterpret_cast<ElfW(Addr)>(__builtin_return_address(0));
  cpp::lock_guard lock(dl::dl_mutex);
  dl::Namespace *parent_namespace = reinterpret_cast<dl::Namespace *>(parent);
  if (parent_namespace == nullptr)
    parent_namespace = dl::namespace_of_address(caller, false);
  else if (!dl::valid_namespace(parent_namespace)) {
    dl::set_error("the parent is not a namespace");
    return nullptr;
  }
  return reinterpret_cast<struct android_namespace_t *>(
      dl::create_namespace(name, ld_library_path, default_library_path, type,
                           permitted_when_isolated_path, parent_namespace));
}

} // namespace LIBC_NAMESPACE_DECL

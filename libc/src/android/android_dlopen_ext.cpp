//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_dlopen_ext.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_dlopen_ext.h"
#include "include/llvm-libc-macros/android-dlext-macros.h"
#include "src/__support/CPP/mutex.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_internal.h"
#include "src/dlfcn/dl_namespace.h"
#include "src/dlfcn/dlopen.h"

namespace LIBC_NAMESPACE_DECL {

// Only a namespace to load into is supported. The other extensions concern
// where the object is placed or read from, and loading it somewhere other
// than asked is worse than not loading it.
LLVM_LIBC_FUNCTION(void *, android_dlopen_ext,
                   (const char *filename, int flags,
                    const android_dlextinfo *extinfo)) {
  const ElfW(Addr) caller =
      reinterpret_cast<ElfW(Addr)>(__builtin_return_address(0));
  dl::Namespace *ns = nullptr;
  if (extinfo != nullptr && extinfo->flags != 0) {
    if ((extinfo->flags &
         ~static_cast<uint64_t>(ANDROID_DLEXT_USE_NAMESPACE)) != 0) {
      dl::set_error("android_dlopen_ext does not support the requested flags");
      return nullptr;
    }
    cpp::lock_guard lock(dl::dl_mutex);
    ns = reinterpret_cast<dl::Namespace *>(extinfo->library_namespace);
    if (ns == nullptr || !dl::valid_namespace(ns)) {
      dl::set_error("ANDROID_DLEXT_USE_NAMESPACE needs a namespace");
      return nullptr;
    }
  }
  return internal::dlopen_in(filename, flags, caller, ns);
}

} // namespace LIBC_NAMESPACE_DECL

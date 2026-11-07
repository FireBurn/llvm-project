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
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_internal.h"
#include "src/dlfcn/dlopen.h"

namespace LIBC_NAMESPACE_DECL {

// The extensions all concern how the object is placed or found: at a reserved
// address, from a descriptor, in a namespace. None of that is done, and
// loading the object somewhere other than asked is worse than not loading it.
LLVM_LIBC_FUNCTION(void *, android_dlopen_ext,
                   (const char *filename, int flags,
                    const android_dlextinfo *extinfo)) {
  if (extinfo != nullptr && extinfo->flags != 0) {
    dl::set_error("android_dlopen_ext does not support the requested flags");
    return nullptr;
  }
  return dlopen(filename, flags);
}

} // namespace LIBC_NAMESPACE_DECL

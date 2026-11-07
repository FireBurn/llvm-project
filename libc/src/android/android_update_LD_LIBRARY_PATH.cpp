//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_update_LD_LIBRARY_PATH.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_update_LD_LIBRARY_PATH.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/setenv.h"

namespace LIBC_NAMESPACE_DECL {

// dlopen reads LD_LIBRARY_PATH from the environment each time it searches.
LLVM_LIBC_FUNCTION(void, android_update_LD_LIBRARY_PATH,
                   (const char *ld_library_path)) {
  if (ld_library_path != nullptr)
    setenv("LD_LIBRARY_PATH", ld_library_path, 1);
}

} // namespace LIBC_NAMESPACE_DECL

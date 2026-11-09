//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_dlwarning.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_dlwarning.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// A loader that has warnings to give hands them to `f` one at a time. This one
// has none: what it would warn about it refuses.
LLVM_LIBC_FUNCTION(void, android_dlwarning,
                   (void *obj, void (*f)(void *, const char *))) {
  (void)obj;
  (void)f;
}

} // namespace LIBC_NAMESPACE_DECL

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_set_16kb_appcompat_mode.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_set_16kb_appcompat_mode.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// The mode makes a loader map libraries aligned for 4KiB pages where the pages
// are 16KiB. This loader maps at the page size it was started with and has
// nothing to switch.
LLVM_LIBC_FUNCTION(void, android_set_16kb_appcompat_mode,
                   (bool enable_app_compat)) {
  (void)enable_app_compat;
}

} // namespace LIBC_NAMESPACE_DECL

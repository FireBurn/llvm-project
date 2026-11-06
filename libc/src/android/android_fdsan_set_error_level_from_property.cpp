//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_fdsan_set_error_level_from_property.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_fdsan_set_error_level_from_property.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/android/fdsan_error_level.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(enum android_fdsan_error_level,
                   android_fdsan_set_error_level_from_property,
                   (enum android_fdsan_error_level default_level)) {
  // There is no property to read, so a level chosen earlier stands and
  // otherwise the default is used.
  if (!fdsan::level_chosen.exchange(true))
    fdsan::error_level.store(static_cast<int>(default_level));
  return static_cast<enum android_fdsan_error_level>(fdsan::error_level.load());
}

} // namespace LIBC_NAMESPACE_DECL

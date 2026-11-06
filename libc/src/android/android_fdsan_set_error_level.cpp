//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_fdsan_set_error_level.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_fdsan_set_error_level.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/android/fdsan_error_level.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(enum android_fdsan_error_level,
                   android_fdsan_set_error_level,
                   (enum android_fdsan_error_level new_level)) {
  fdsan::level_chosen.store(true);
  return static_cast<enum android_fdsan_error_level>(
      fdsan::error_level.exchange(static_cast<int>(new_level)));
}

} // namespace LIBC_NAMESPACE_DECL

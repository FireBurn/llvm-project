//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_fdsan_get_owner_tag.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_fdsan_get_owner_tag.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(uint64_t, android_fdsan_get_owner_tag, (int fd)) {
  // Descriptors are not tracked, so none has an owner.
  (void)fd;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

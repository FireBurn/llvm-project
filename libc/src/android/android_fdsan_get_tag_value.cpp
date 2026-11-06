//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_fdsan_get_tag_value.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_fdsan_get_tag_value.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(uint64_t, android_fdsan_get_tag_value, (uint64_t tag)) {
  return tag & ((uint64_t(1) << 56) - 1);
}

} // namespace LIBC_NAMESPACE_DECL

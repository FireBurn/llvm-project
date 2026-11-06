//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_fdsan_exchange_owner_tag.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_fdsan_exchange_owner_tag.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, android_fdsan_exchange_owner_tag,
                   (int fd, uint64_t expected_tag, uint64_t new_tag)) {
  // Descriptors are not tracked, so there is no owner to exchange.
  (void)fd;
  (void)expected_tag;
  (void)new_tag;
}

} // namespace LIBC_NAMESPACE_DECL

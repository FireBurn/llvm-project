//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for android_fdsan_exchange_owner_tag.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ANDROID_ANDROID_FDSAN_EXCHANGE_OWNER_TAG_H
#define LLVM_LIBC_SRC_ANDROID_ANDROID_FDSAN_EXCHANGE_OWNER_TAG_H

#include "hdr/stdint_proxy.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

void android_fdsan_exchange_owner_tag(int fd, uint64_t expected_tag,
                                      uint64_t new_tag);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ANDROID_ANDROID_FDSAN_EXCHANGE_OWNER_TAG_H

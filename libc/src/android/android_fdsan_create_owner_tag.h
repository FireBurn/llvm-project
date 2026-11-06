//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for android_fdsan_create_owner_tag.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ANDROID_ANDROID_FDSAN_CREATE_OWNER_TAG_H
#define LLVM_LIBC_SRC_ANDROID_ANDROID_FDSAN_CREATE_OWNER_TAG_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/enum_android_fdsan_owner_type.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

uint64_t android_fdsan_create_owner_tag(enum android_fdsan_owner_type type,
                                        uint64_t tag);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ANDROID_ANDROID_FDSAN_CREATE_OWNER_TAG_H

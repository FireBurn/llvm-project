//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for android_crash_detail_replace_name.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ANDROID_ANDROID_CRASH_DETAIL_REPLACE_NAME_H
#define LLVM_LIBC_SRC_ANDROID_ANDROID_CRASH_DETAIL_REPLACE_NAME_H

#include "src/__support/crash_detail.h"
#include "src/__support/macros/config.h"

#include <stddef.h>

namespace LIBC_NAMESPACE_DECL {

void android_crash_detail_replace_name(crash_detail::Detail *detail,
                                       const void *name, size_t name_size);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ANDROID_ANDROID_CRASH_DETAIL_REPLACE_NAME_H

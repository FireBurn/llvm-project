//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_crash_detail_replace_name.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_crash_detail_replace_name.h"
#include "src/__support/common.h"
#include "src/__support/crash_detail.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, android_crash_detail_replace_name,
                   (crash_detail::Detail * detail, const void *name,
                    size_t name_size)) {
  crash_detail::replace_name(detail, name, name_size);
}

} // namespace LIBC_NAMESPACE_DECL

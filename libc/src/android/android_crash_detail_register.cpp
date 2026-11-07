//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_crash_detail_register.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_crash_detail_register.h"
#include "src/__support/common.h"
#include "src/__support/crash_detail.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(crash_detail::Detail *, android_crash_detail_register,
                   (const void *name, size_t name_size, const void *data,
                    size_t data_size)) {
  return crash_detail::add(name, name_size, data, data_size);
}

} // namespace LIBC_NAMESPACE_DECL

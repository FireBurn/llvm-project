//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_crash_detail_replace_data.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_crash_detail_replace_data.h"
#include "src/__support/common.h"
#include "src/__support/crash_detail.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, android_crash_detail_replace_data,
                   (crash_detail::Detail * detail, const void *data,
                    size_t data_size)) {
  crash_detail::replace_data(detail, data, data_size);
}

} // namespace LIBC_NAMESPACE_DECL

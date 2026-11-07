//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of strerrorname_np.
///
//===----------------------------------------------------------------------===//

#include "src/string/strerrorname_np.h"
#include "src/__support/StringUtil/error_to_string.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(const char *, strerrorname_np, (int err_num)) {
  auto name = try_get_errno_name(err_num);
  // The names are stored with their terminator.
  return name ? name->data() : nullptr;
}

} // namespace LIBC_NAMESPACE_DECL

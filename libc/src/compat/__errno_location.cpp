//===-- Implementation of __errno_location --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/compat/__errno_location.h"

#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// What errno is on this platform is settled by the ABI rather than by the
// header a caller happened to compile against: anything already built, and
// anything that states its own declarations rather than including errno.h,
// reaches for this name. It is the same location __llvm_libc_errno gives.
LLVM_LIBC_FUNCTION(int *, __errno_location, (void)) {
  return __llvm_libc_errno();
}

} // namespace LIBC_NAMESPACE_DECL

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __errno.
///
//===----------------------------------------------------------------------===//

#include "src/android/__errno.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// bionic's errno macro expands to a call of this.
LLVM_LIBC_FUNCTION(int *, __errno, ()) { return &libc_errno; }

} // namespace LIBC_NAMESPACE_DECL

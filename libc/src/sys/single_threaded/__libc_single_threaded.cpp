//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of __libc_single_threaded
///
//===----------------------------------------------------------------------===//

#include "src/sys/single_threaded/__libc_single_threaded.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Cleared when the first thread is created, and never set again.
LLVM_LIBC_VARIABLE(char, __libc_single_threaded) = 1;

} // namespace LIBC_NAMESPACE_DECL

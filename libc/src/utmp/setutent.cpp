//===-- Implementation of setutent ------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/utmp/setutent.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/utmpx/utmpx_db.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, setutent, (void)) { utmpx_db::rewind(); }

} // namespace LIBC_NAMESPACE_DECL

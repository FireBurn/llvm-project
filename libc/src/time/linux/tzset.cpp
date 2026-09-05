//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the definition of the tzset function.
///
//===----------------------------------------------------------------------===//

#include "src/time/tzset.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/time/tz/timezone.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, tzset, ()) { tz::set_from_environment(); }

} // namespace LIBC_NAMESPACE_DECL

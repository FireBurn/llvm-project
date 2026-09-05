//===-- Implementation of setsgent ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/gshadow/setsgent.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/gshadow/gshadow_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, setsgent, ()) {
  // There is no way to report a failure here, the same as setgrent.
  gshadow_db::open();
}

} // namespace LIBC_NAMESPACE_DECL

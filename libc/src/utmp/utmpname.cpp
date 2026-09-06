//===-- Implementation of utmpname ------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/utmp/utmpname.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/utmpx/utmpx_db.h"

namespace LIBC_NAMESPACE_DECL {

// The same file the utmpx calls read, so pointing one of the two elsewhere
// points both.
LLVM_LIBC_FUNCTION(int, utmpname, (const char *file)) {
  if (file == nullptr)
    return -1;
  utmpx_db::close();
  utmpx_db::path = file;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

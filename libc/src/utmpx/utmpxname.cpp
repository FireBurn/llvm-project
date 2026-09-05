//===-- Implementation of utmpxname ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/utmpx/utmpxname.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/utmpx/utmpx_db.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, utmpxname, (const char *file)) {
  if (file == nullptr)
    return -1;

  // Any iteration under way was on the old file, so it is dropped. The
  // string is not copied: the caller has to keep it around, which is what
  // glibc asks of it too.
  utmpx_db::close();
  utmpx_db::path = file;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL

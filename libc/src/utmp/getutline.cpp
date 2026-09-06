//===-- Implementation of getutline -----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/utmp/getutline.h"

#include "hdr/types/struct_utmp.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/utmp/utmp_record.h"
#include "src/utmpx/getutxline.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(struct utmp *, getutline, (const struct utmp *line)) {
  return as_utmp(LIBC_NAMESPACE::getutxline(as_utmpx(line)));
}

} // namespace LIBC_NAMESPACE_DECL

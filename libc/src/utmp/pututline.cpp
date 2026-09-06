//===-- Implementation of pututline -----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/utmp/pututline.h"

#include "hdr/types/struct_utmp.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/utmp/utmp_record.h"
#include "src/utmpx/pututxline.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(struct utmp *, pututline, (const struct utmp *utmp)) {
  return as_utmp(LIBC_NAMESPACE::pututxline(as_utmpx(utmp)));
}

} // namespace LIBC_NAMESPACE_DECL

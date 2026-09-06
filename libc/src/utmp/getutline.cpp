//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of getutline
///
//===----------------------------------------------------------------------===//

#include "src/utmp/getutline.h"

#include "hdr/types/struct_utmp.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/utmp/utmp_record.h"
#include "src/utmpx/getutxline.h"

namespace LIBC_NAMESPACE_DECL {

namespace internal {

struct utmp *getutline_impl(const struct utmp *line) {
  return as_utmp(internal::getutxline_impl(as_utmpx(line)));
}

} // namespace internal

LLVM_LIBC_FUNCTION(struct utmp *, getutline, (const struct utmp *line)) {
  return internal::getutline_impl(line);
}

} // namespace LIBC_NAMESPACE_DECL

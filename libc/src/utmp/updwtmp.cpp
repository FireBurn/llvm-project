//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of updwtmp
///
//===----------------------------------------------------------------------===//

#include "src/utmp/updwtmp.h"

#include "hdr/types/struct_utmp.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/utmp/utmp_record.h"
#include "src/utmpx/updwtmpx.h"

namespace LIBC_NAMESPACE_DECL {

namespace internal {

void updwtmp_impl(const char *wtmp_file, const struct utmp *utmp) {
  internal::updwtmpx_impl(wtmp_file, as_utmpx(utmp));
}

} // namespace internal

LLVM_LIBC_FUNCTION(void, updwtmp,
                   (const char *wtmp_file, const struct utmp *utmp)) {
  internal::updwtmp_impl(wtmp_file, utmp);
}

} // namespace LIBC_NAMESPACE_DECL

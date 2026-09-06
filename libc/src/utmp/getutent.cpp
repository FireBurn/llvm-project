//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of getutent
///
//===----------------------------------------------------------------------===//

#include "src/utmp/getutent.h"

#include "hdr/types/struct_utmp.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/utmp/utmp_record.h"
#include "src/utmpx/utmpx_db.h"

namespace LIBC_NAMESPACE_DECL {

namespace internal {

struct utmp *getutent_impl(void) {
  if (!utmpx_db::read_next())
    return nullptr;
  return as_utmp(&utmpx_db::entry);
}

} // namespace internal

LLVM_LIBC_FUNCTION(struct utmp *, getutent, (void)) {
  return internal::getutent_impl();
}

} // namespace LIBC_NAMESPACE_DECL

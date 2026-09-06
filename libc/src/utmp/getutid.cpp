//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of getutid
///
//===----------------------------------------------------------------------===//

#include "src/utmp/getutid.h"

#include "hdr/types/struct_utmp.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/utmp/utmp_record.h"
#include "src/utmpx/getutxid.h"

namespace LIBC_NAMESPACE_DECL {

namespace internal {

struct utmp *getutid_impl(const struct utmp *id) {
  return as_utmp(internal::getutxid_impl(as_utmpx(id)));
}

} // namespace internal

LLVM_LIBC_FUNCTION(struct utmp *, getutid, (const struct utmp *id)) {
  return internal::getutid_impl(id);
}

} // namespace LIBC_NAMESPACE_DECL

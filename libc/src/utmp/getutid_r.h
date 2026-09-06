//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for getutid_r
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_UTMP_GETUTID_R_H
#define LLVM_LIBC_SRC_UTMP_GETUTID_R_H

#include "hdr/types/struct_utmp.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int getutid_r(const struct utmp *id, struct utmp *buffer, struct utmp **result);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_UTMP_GETUTID_R_H

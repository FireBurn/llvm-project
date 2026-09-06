//===-- The record shared with utmpx ----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_UTMP_UTMP_RECORD_H
#define LLVM_LIBC_SRC_UTMP_UTMP_RECORD_H

#include "hdr/types/struct_utmp.h"
#include "hdr/types/struct_utmpx.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// struct utmp and struct utmpx are one record under two names, so the calls
// of both interfaces read and write the same file through the same code.
// Only the name a caller sees differs.
static_assert(sizeof(struct utmp) == sizeof(struct utmpx),
              "struct utmp and struct utmpx have to be the same record");

LIBC_INLINE struct utmp *as_utmp(struct utmpx *record) {
  return reinterpret_cast<struct utmp *>(record);
}

LIBC_INLINE const struct utmpx *as_utmpx(const struct utmp *record) {
  return reinterpret_cast<const struct utmpx *>(record);
}

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_UTMP_UTMP_RECORD_H

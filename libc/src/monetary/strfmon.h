//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for strfmon
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_MONETARY_STRFMON_H
#define LLVM_LIBC_SRC_MONETARY_STRFMON_H

#include "hdr/types/locale_t.h"
#include "hdr/types/size_t.h"
#include "hdr/types/ssize_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

ssize_t strfmon(char *__restrict s, size_t maxsize,
                const char *__restrict format, ...);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_MONETARY_STRFMON_H

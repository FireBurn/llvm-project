//===-- Implementation header for parse_printf_format -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_PRINTF_PARSE_PRINTF_FORMAT_H
#define LLVM_LIBC_SRC_PRINTF_PARSE_PRINTF_FORMAT_H

#include "hdr/types/size_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

size_t parse_printf_format(const char *__restrict format, size_t n,
                           int *__restrict argtypes);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_PRINTF_PARSE_PRINTF_FORMAT_H

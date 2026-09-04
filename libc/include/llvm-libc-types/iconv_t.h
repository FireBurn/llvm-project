//===-- Definition of iconv_t type ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_ICONV_T_H
#define LLVM_LIBC_TYPES_ICONV_T_H

// An open conversion, which only iconv_open produces and only iconv and
// iconv_close may look at.
typedef void *iconv_t;

#endif // LLVM_LIBC_TYPES_ICONV_T_H

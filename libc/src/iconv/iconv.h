//===-- Implementation header for iconv ------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ICONV_ICONV_H
#define LLVM_LIBC_SRC_ICONV_ICONV_H

#include "hdr/types/iconv_t.h"
#include "hdr/types/size_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

size_t iconv(iconv_t cd, char **__restrict inbuf,
             size_t *__restrict inbytesleft, char **__restrict outbuf,
             size_t *__restrict outbytesleft);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ICONV_ICONV_H

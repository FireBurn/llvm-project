//===-- Implementation header for fgetspent_r -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SHADOW_FGETSPENT_R_H
#define LLVM_LIBC_SRC_SHADOW_FGETSPENT_R_H

#include "hdr/types/FILE.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_spwd.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int fgetspent_r(::FILE *stream, struct spwd *spbuf, char *buf, size_t buflen,
                struct spwd **spbufp);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SHADOW_FGETSPENT_R_H

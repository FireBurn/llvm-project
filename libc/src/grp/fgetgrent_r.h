//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for fgetgrent_r
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_GRP_FGETGRENT_R_H
#define LLVM_LIBC_SRC_GRP_FGETGRENT_R_H

#include "hdr/types/FILE.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_group.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int fgetgrent_r(::FILE *stream, struct group *grbuf, char *buf, size_t buflen,
                struct group **grbufp);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_GRP_FGETGRENT_R_H

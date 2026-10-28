//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for sgetsgent_r
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_GSHADOW_SGETSGENT_R_H
#define LLVM_LIBC_SRC_GSHADOW_SGETSGENT_R_H

#include "hdr/types/size_t.h"
#include "hdr/types/struct_sgrp.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int sgetsgent_r(const char *s, struct sgrp *sgbuf, char *buf, size_t buflen,
                struct sgrp **sgbufp);

namespace internal {
int sgetsgent_r_impl(const char *s, struct sgrp *sgbuf, char *buf,
                     size_t buflen, struct sgrp **sgbufp);
} // namespace internal

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_GSHADOW_SGETSGENT_R_H

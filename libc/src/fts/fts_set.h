//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for fts_set.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_FTS_FTS_SET_H
#define LLVM_LIBC_SRC_FTS_FTS_SET_H

#include "hdr/types/FTS.h"
#include "hdr/types/FTSENT.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int fts_set(FTS *fts, FTSENT *entry, int options);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_FTS_FTS_SET_H

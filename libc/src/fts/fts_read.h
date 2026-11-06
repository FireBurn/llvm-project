//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for fts_read.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_FTS_FTS_READ_H
#define LLVM_LIBC_SRC_FTS_FTS_READ_H

#include "hdr/types/FTS.h"
#include "hdr/types/FTSENT.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

FTSENT *fts_read(FTS *fts);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_FTS_FTS_READ_H

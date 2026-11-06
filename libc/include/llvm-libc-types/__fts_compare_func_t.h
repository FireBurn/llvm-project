//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of __fts_compare_func_t.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_FTS_COMPARE_FUNC_T_H
#define LLVM_LIBC_TYPES_FTS_COMPARE_FUNC_T_H

#include "FTSENT.h"

typedef int (*__fts_compare_func_t)(const FTSENT **, const FTSENT **);

#endif // LLVM_LIBC_TYPES_FTS_COMPARE_FUNC_T_H

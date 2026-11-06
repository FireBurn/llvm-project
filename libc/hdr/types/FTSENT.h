//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Proxy for FTSENT.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_HDR_TYPES_FTSENT_H
#define LLVM_LIBC_HDR_TYPES_FTSENT_H

#ifdef LIBC_FULL_BUILD

#include "include/llvm-libc-types/FTSENT.h"

#else // Overlay mode

#include <fts.h>

#endif // LIBC_FULL_BUILD

#endif // LLVM_LIBC_HDR_TYPES_FTSENT_H

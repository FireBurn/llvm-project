//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of macros from sys/limits.h.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SYS_LIMITS_MACROS_H
#define LLVM_LIBC_MACROS_SYS_LIMITS_MACROS_H

#include "limits-macros.h"

// The largest user and group ids, which are 32 bits wide on Linux.
#ifndef UID_MAX
#define UID_MAX UINT_MAX
#endif

#ifndef GID_MAX
#define GID_MAX UINT_MAX
#endif

#endif // LLVM_LIBC_MACROS_SYS_LIMITS_MACROS_H

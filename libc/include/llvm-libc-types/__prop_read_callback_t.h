//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of __prop_read_callback_t.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_PROP_READ_CALLBACK_T_H
#define LLVM_LIBC_TYPES_PROP_READ_CALLBACK_T_H

#include "../llvm-libc-macros/stdint-macros.h"

typedef void (*__prop_read_callback_t)(void *, const char *, const char *,
                                       uint32_t);

#endif // LLVM_LIBC_TYPES_PROP_READ_CALLBACK_T_H

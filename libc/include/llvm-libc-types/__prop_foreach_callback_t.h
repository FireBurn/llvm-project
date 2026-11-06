//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of __prop_foreach_callback_t.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_PROP_FOREACH_CALLBACK_T_H
#define LLVM_LIBC_TYPES_PROP_FOREACH_CALLBACK_T_H

#include "prop_info.h"

typedef void (*__prop_foreach_callback_t)(const prop_info *, void *);

#endif // LLVM_LIBC_TYPES_PROP_FOREACH_CALLBACK_T_H

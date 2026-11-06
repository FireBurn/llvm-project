//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Proxy for __prop_read_callback_t.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_HDR_TYPES_PROP_READ_CALLBACK_T_H
#define LLVM_LIBC_HDR_TYPES_PROP_READ_CALLBACK_T_H

#ifdef LIBC_FULL_BUILD

#include "include/llvm-libc-types/__prop_read_callback_t.h"

#else // Overlay mode

#include <sys/system_properties.h>

#endif // LIBC_FULL_BUILD

#endif // LLVM_LIBC_HDR_TYPES_PROP_READ_CALLBACK_T_H

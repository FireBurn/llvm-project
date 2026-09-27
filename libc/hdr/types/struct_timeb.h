//===-- Proxy for struct timeb --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_HDR_TYPES_STRUCT_TIMEB_H
#define LLVM_LIBC_HDR_TYPES_STRUCT_TIMEB_H

#ifdef LIBC_FULL_BUILD

#include "include/llvm-libc-types/struct_timeb.h"

#else // Overlay mode

#include <sys/timeb.h>

#endif // LIBC_FULL_BUILD

#endif // LLVM_LIBC_HDR_TYPES_STRUCT_TIMEB_H

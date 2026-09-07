//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of n_time.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_N_TIME_H
#define LLVM_LIBC_TYPES_N_TIME_H

#include "../llvm-libc-macros/stdint-macros.h"

// Milliseconds since midnight UTC, as an IP timestamp option carries it.
typedef uint32_t n_time;

#endif // LLVM_LIBC_TYPES_N_TIME_H

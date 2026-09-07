//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of n_long.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_N_LONG_H
#define LLVM_LIBC_TYPES_N_LONG_H

#include "../llvm-libc-macros/stdint-macros.h"

// A long as it arrives from the network, thirty two bits wide whatever a long
// is on this machine.
typedef uint32_t n_long;

#endif // LLVM_LIBC_TYPES_N_LONG_H

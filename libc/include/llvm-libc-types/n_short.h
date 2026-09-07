//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of n_short.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_N_SHORT_H
#define LLVM_LIBC_TYPES_N_SHORT_H

#include "../llvm-libc-macros/stdint-macros.h"

// A short as it arrives from the network. BSD wrote these when a short was
// sixteen bits and a long was thirty two, and the names outlived the
// assumption, so the width is stated rather than borrowed.
typedef uint16_t n_short;

#endif // LLVM_LIBC_TYPES_N_SHORT_H

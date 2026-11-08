//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of uint_t type.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_UINT_T_H
#define LLVM_LIBC_TYPES_UINT_T_H

// Only bionic exposes this name.
#ifdef __ANDROID__
typedef unsigned int uint_t;
#endif

#endif // LLVM_LIBC_TYPES_UINT_T_H

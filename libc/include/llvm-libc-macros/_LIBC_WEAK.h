//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the definition of the _LIBC_WEAK macro.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LIBC_WEAK_H
#define LLVM_LIBC_MACROS_LIBC_WEAK_H

#define _LIBC_WEAK __attribute__((weak))

#endif // LLVM_LIBC_MACROS_LIBC_WEAK_H

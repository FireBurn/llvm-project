//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the definition of the _LIBC_FORMAT_ARG macro.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LIBC_FORMAT_ARG_H
#define LLVM_LIBC_MACROS_LIBC_FORMAT_ARG_H

#define _LIBC_FORMAT_ARG(INDEX) __attribute__((format_arg(INDEX)))

#endif // LLVM_LIBC_MACROS_LIBC_FORMAT_ARG_H

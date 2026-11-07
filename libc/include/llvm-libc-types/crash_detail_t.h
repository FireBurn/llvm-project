//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of crash_detail_t.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_CRASH_DETAIL_T_H
#define LLVM_LIBC_TYPES_CRASH_DETAIL_T_H

// What is behind the name is private. A crash handler reads the details out of
// the memory of the process that crashed.
typedef struct crash_detail_t crash_detail_t;

#endif // LLVM_LIBC_TYPES_CRASH_DETAIL_T_H

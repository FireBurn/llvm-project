//===-- Definition of lwpid_t ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_LWPID_T_H
#define LLVM_LIBC_TYPES_LWPID_T_H

#include "pid_t.h"

// A thread, named the way the process notes name it.
typedef pid_t lwpid_t;

#endif // LLVM_LIBC_TYPES_LWPID_T_H

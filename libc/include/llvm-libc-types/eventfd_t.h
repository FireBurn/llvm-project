//===-- Definition of type eventfd_t --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_EVENTFD_T_H
#define LLVM_LIBC_TYPES_EVENTFD_T_H

#include "../llvm-libc-macros/stdint-macros.h"

// The counter an eventfd holds, which is what a read takes from and a write
// adds to.
typedef uint64_t eventfd_t;

#endif // LLVM_LIBC_TYPES_EVENTFD_T_H

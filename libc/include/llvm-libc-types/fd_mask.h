//===-- Definition of fd_mask type ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_FD_MASK_H
#define LLVM_LIBC_TYPES_FD_MASK_H

#include "../llvm-libc-macros/sys-select-macros.h" // __FD_SET_WORD_TYPE

// One word of an fd_set. It is the type of fds_bits, so a pointer to one can
// walk the set.
typedef __FD_SET_WORD_TYPE fd_mask;

#endif // LLVM_LIBC_TYPES_FD_MASK_H

//===-- Macros defined in sys/swap.h header file --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SYS_SWAP_MACROS_H
#define LLVM_LIBC_MACROS_SYS_SWAP_MACROS_H

// The flags swapon takes. A priority is given by setting SWAP_FLAG_PREFER
// and putting the value in the bits SWAP_FLAG_PRIO_MASK covers.
#define SWAP_FLAG_PREFER 0x8000
#define SWAP_FLAG_PRIO_MASK 0x7fff
#define SWAP_FLAG_PRIO_SHIFT 0
#define SWAP_FLAG_DISCARD 0x10000

#endif // LLVM_LIBC_MACROS_SYS_SWAP_MACROS_H

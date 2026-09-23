//===-- Definition of __barrier_type type ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES__BARRIER_TYPE_H
#define LLVM_LIBC_TYPES__BARRIER_TYPE_H

// glibc's size and alignment. The implementation needs about twenty bytes.
typedef struct __attribute__((aligned(8))) {
  char __size[32];
} __barrier_type;

#endif // LLVM_LIBC_TYPES__BARRIER_TYPE_H

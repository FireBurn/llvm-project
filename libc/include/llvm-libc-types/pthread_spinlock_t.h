//===-- Definition of pthread_spinlock_t type -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_PTHREAD_SPINLOCK_T_H
#define LLVM_LIBC_TYPES_PTHREAD_SPINLOCK_T_H

// The thread id of the holder, or zero when free. glibc's type is an int.
typedef int pthread_spinlock_t;

#endif // LLVM_LIBC_TYPES_PTHREAD_SPINLOCK_T_H

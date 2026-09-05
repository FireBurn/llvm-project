//===-- Implementation header for pthread_getattr_np ------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_PTHREAD_PTHREAD_GETATTR_NP_H
#define LLVM_LIBC_SRC_PTHREAD_PTHREAD_GETATTR_NP_H

#include "src/__support/macros/config.h"
#include <pthread.h>

namespace LIBC_NAMESPACE_DECL {

// Fills in the attributes a thread is actually running with, rather than the
// ones it was asked for. The stack of the first thread grows as it is used,
// so what is reported for it is the room it is allowed rather than the room
// it has taken.
int pthread_getattr_np(pthread_t th, pthread_attr_t *attr);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_PTHREAD_PTHREAD_GETATTR_NP_H

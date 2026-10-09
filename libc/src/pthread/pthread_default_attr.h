//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The attributes new threads get by default
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_PTHREAD_PTHREAD_DEFAULT_ATTR_H
#define LLVM_LIBC_SRC_PTHREAD_PTHREAD_DEFAULT_ATTR_H

#include "hdr/types/pthread_attr_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// The attributes pthread_create gives a thread when it is not given any, which
// pthread_setattr_default_np changes.
pthread_attr_t default_thread_attr();
void set_default_thread_attr(const pthread_attr_t &attr);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_PTHREAD_PTHREAD_DEFAULT_ATTR_H

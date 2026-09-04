//===-- Implementation header for eventfd -----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_EVENTFD_EVENTFD_H
#define LLVM_LIBC_SRC_SYS_EVENTFD_EVENTFD_H

#include "hdr/types/eventfd_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int eventfd(unsigned int initval, int flags);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_EVENTFD_EVENTFD_H
